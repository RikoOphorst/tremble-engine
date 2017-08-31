#include "stack_allocator.h"

namespace tremble
{

	//------------------------------------------------------------------------------------------------------
	StackAllocator::StackAllocator(size_t size, void* start)
		:Allocator(size, start), current_pos_(start)
	{
		ASSERT(size > 0);
#if _DEBUG
		prev_position_ = nullptr;
#endif
	}

	//------------------------------------------------------------------------------------------------------
	StackAllocator::~StackAllocator()
	{
#if _DEBUG
		prev_position_ = nullptr;
#endif
		current_pos_ = nullptr;
	}

	//------------------------------------------------------------------------------------------------------
	void* StackAllocator::Allocate(size_t size, u8 alignment)
	{
        allocator_lock_.lock();
		ASSERT(size != 0);

		//calculate the adjustment, needed for alignment + header 
		u8 adjustment = pointer_math::AlignForwardAdjustmentWithHeader(current_pos_, alignment, sizeof(AllocationHeader));

		if (used_memory_ + adjustment + size > size_)
		{
			DLOG("New allocation inside a stack allocator could not complete. Not enough memory in the allocator")
			return nullptr;
		}

		void* aligned_address = pointer_math::Add(current_pos_, adjustment);

		//Add allocation header
		AllocationHeader* header = (AllocationHeader*)(pointer_math::Substract(aligned_address, sizeof(AllocationHeader)));

		header->adjustment = adjustment;

#if _DEBUG
		header->prev_address = prev_position_;
		prev_position_ = aligned_address;
#endif

		current_pos_ = pointer_math::Add(aligned_address, size);

		used_memory_ += size + adjustment;
		num_allocations_++;

        allocator_lock_.unlock();
		return aligned_address;
	}

	//------------------------------------------------------------------------------------------------------
	void StackAllocator::Deallocate(void* p)
	{
        allocator_lock_.lock();
		//Access the allocation header in the bytes before p
		AllocationHeader* header = (AllocationHeader*)(pointer_math::Substract(p, sizeof(AllocationHeader)));


		used_memory_ -= (uptr)current_pos_ - (uptr)p + header->adjustment;

		current_pos_ = pointer_math::Substract(p, header->adjustment);

#if _DEBUG
		//When deallocating, you have to make sure to deallocate just the top objet in the stack. In debug mode it is checked here, but in release it's not
		ASSERT(p == prev_position_);
		prev_position_ = header->prev_address;
#endif
		num_allocations_--;
        allocator_lock_.unlock();
	}
}