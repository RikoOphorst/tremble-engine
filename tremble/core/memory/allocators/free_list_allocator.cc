#include "free_list_allocator.h"
#include "../../utilities/debug.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	FreeListAllocator::FreeListAllocator(size_t size, void* start)
		:Allocator(size, start), free_blocks_((FreeBlock*)start)
	{
		ASSERT(size > sizeof(FreeBlock));

		free_blocks_->size = size;
		free_blocks_->next = nullptr;
	}

	//------------------------------------------------------------------------------------------------------
	FreeListAllocator::~FreeListAllocator()
	{
		free_blocks_ = nullptr;
	}

	//------------------------------------------------------------------------------------------------------
	void* FreeListAllocator::Allocate(size_t size, u8 alignment)
	{
        allocator_lock_.lock();
		ASSERT(size != 0 && alignment != 0);

		FreeBlock* prev_free_block = nullptr;
		FreeBlock* free_block = free_blocks_;

		//scroll through all the free blocks 
		while (free_block != nullptr)
		{
			//calculate adjustment needed for alignment
			u8 adjustment = pointer_math::AlignForwardAdjustmentWithHeader(free_block, alignment, sizeof(AllocationHeader));

			size_t total_size = size + adjustment;

			//If there is not enough space in this block
			if (free_block->size < total_size)
			{
				//Skip through to the next one
				prev_free_block = free_block;
				free_block = free_block->next;
				continue;
			}

			//Assert at compile time 
			static_assert(sizeof(AllocationHeader) >= sizeof(FreeBlock), "sizeof(AllocationHeader) < sizeof(FreeBlock)");

			//If we found a free block that can fit the needed data, but nothing more
			if (free_block->size - total_size <= sizeof(AllocationHeader))
			{
				//Increase allocation size to fill up the free block in whole. 
				total_size = free_block->size;

				if (prev_free_block != nullptr)
				{
                    ASSERT(prev_free_block->next != prev_free_block);
					prev_free_block->next = free_block->next;
				}
				else
				{
					free_blocks_ = free_block->next;
				}
			}
			else
			{
				//Create a new free block, containing remaining memory
				FreeBlock* next_block = (FreeBlock*)(pointer_math::Add(free_block, total_size));
				next_block->size = free_block->size - total_size;
                ASSERT(free_block->next != next_block);
				next_block->next = free_block->next;

				if (prev_free_block != nullptr)
				{
                    ASSERT(prev_free_block != next_block);
					prev_free_block->next = next_block;
				}
				else
				{
					free_blocks_ = next_block;
				}
			}
			uptr aligned_address = (uptr)free_block + adjustment;

			AllocationHeader* header = (AllocationHeader*)(aligned_address - sizeof(AllocationHeader));

			header->size = total_size;
			header->adjustment = adjustment;

			used_memory_ += total_size;
			num_allocations_++;

			ASSERT(pointer_math::AlignForwardAdjustment((void*)aligned_address, alignment) == 0);

            allocator_lock_.unlock();
			return (void*)aligned_address;
		}

        allocator_lock_.unlock();
		DLOG("Memory could not be allocated inside a free list allocator. Could not find a free block large enough");
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------
	void FreeListAllocator::Deallocate(void* p)
	{
        allocator_lock_.lock();
		ASSERT(p != nullptr && "Trying to deallocate a nullptr");
		AllocationHeader* header = (AllocationHeader*)pointer_math::Substract(p, sizeof(AllocationHeader));

		uptr block_start = reinterpret_cast<uptr>(p) - header->adjustment;
		size_t block_size = header->size;
		uptr block_end = block_start + block_size;

		FreeBlock* prev_free_block = nullptr;

		FreeBlock* free_block = free_blocks_;
		
		//go through all the free blocks
		while (free_block != nullptr)
		{
			//until we stumble upon the next free block after the one currently being deallocated, or we pass all the free blocks
			if ((uptr)free_block >= block_end)
				break;

			prev_free_block = free_block;
			free_block = free_block->next;
		}

		//If there are no free block in memory before the one, currently being deallocated
		if (prev_free_block == nullptr)
		{
			//Make this the first free block in the allocator
			prev_free_block = (FreeBlock*)block_start;
			prev_free_block->size = block_size;
            ASSERT(free_blocks_ != prev_free_block);
			prev_free_block->next = free_blocks_; 

			free_blocks_ = prev_free_block;
		}
		//if previous free block is located right before the one, being deallocated
		else if ((uptr)prev_free_block + prev_free_block->size == block_start)
		{
			//Extend the previous free block to extend this one
			prev_free_block->size += block_size;
		}
		//if there is a previous free block, but it is not next to the one being deallocated
		else
		{
			//Create a new free block at the location of the one, being deallocated
			FreeBlock* temp = (FreeBlock*)block_start;
			temp->size = block_size;
            ASSERT(temp != prev_free_block->next);
			temp->next = prev_free_block->next;
            ASSERT(temp != prev_free_block);
			prev_free_block->next = temp;

			prev_free_block = temp;
		}
		//if there is a free block right next to this one later in memory
		if (free_block != nullptr && (uptr)free_block == block_end)
		{
			//Merge the next free block to this one (by this point prev_free_block is guaranteed to be the one, being deallocated)
			prev_free_block->size += free_block->size;
            ASSERT(prev_free_block != free_block->next);
			prev_free_block->next = free_block->next;
		}

		num_allocations_--;
		used_memory_ -= block_size;
        allocator_lock_.unlock();
	}
}