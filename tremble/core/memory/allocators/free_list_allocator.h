#pragma once

#include "allocator.h"

namespace tremble
{
	/**
	* @class tremble::FreeListAllocator
	* @brief A free list allocator class
	*
	* A general allocator class that can be used in all situations. Upon allocation it allocates a block of required size. 
	* Upon deallocation it marks the block as free again
	* Each block contains a header, that indicates the size of the block and the size of adjustment, used for alignment.
	* This allocator is prone to fragmentation, so even though it can be used anywhere, it might be wise to think if any other one might be better.
	* Right now to find a free block for an allocation, it uses a first-fit strategy. It is the fastest way of doing it, but it is not the best one, as it can become fragmentated quickly
	* A better algorithms may be introduced later during the development to use a best-fit strategy.
	*
	* @author Anton Gavrilov
	*/
	class FreeListAllocator : public Allocator
	{
	public:

		/**
		* @brief Free List Allocator constructor
		*
		* @param size Size of memory, reserved for this allocator
		* @param start Pointer to the memory at the start of the allocator
		*/
		FreeListAllocator(size_t size, void* start);
		~FreeListAllocator(); //!< Free list allocator destructor

		/**
		* @brief Allocate some memory inside of this free list allocator
		* @param size Size of the memory, that you want to allocate
		* @param alingment Desired alignment of the memory. 4 by default
		* @return Raw pointer to the memory address of the allocated memory
		*/
		void* Allocate(size_t size, u8 alignment = 4) override;

		/**
		* @brief Deallocate a piece of memory
		* @param p Pointer to the piece of memory, that you wish to deallocate
		*/
		void Deallocate(void* p) override;

		/**
		* @brief Returns the overhead per allocation made with this allocator
		*/
		static unsigned int GetOverheadPerAllocation()
		{
			return static_cast<unsigned int>(sizeof(AllocationHeader));
		}

		/**
		* @brief Allows you to query for the recommended memory pool size of this allocator if you know what you're going to be allocating
		* @param[in] size_of_object The size of the object that you will be allocating in this allocator
		* @param[in] expected_allocation_count The expected number of allocations of size_of_object you're going to be doing with this allocator
		*/
		static unsigned int GetRecommendedMemoryPoolSize(size_t size_of_object, unsigned int expected_allocation_count)
		{
			// @TODO fix this properly - it shouldn't just increase by one but do a proper approximation of the recommended memp size
			expected_allocation_count += 1; // increase by 1 to offset any >= or <= faults
			return static_cast<unsigned int>(expected_allocation_count * size_of_object + expected_allocation_count * sizeof(AllocationHeader));
		}

	private:
		/**
		* @struct AllocationHeader
		* @brief Allocation header for the free list allocator class
		*/
		struct AllocationHeader
		{
			size_t size; //!< Size of the data in the block
			u8 adjustment; //!< Adjustment size, used to align the data & fit the header before the data
		};
		
		/**
		* @struct tremble::FreeListAllocator::FreeBlock
		* @brief An object which represents a free, unallocated block of memory in this allocator. Functions as a linked list
		*/
		struct FreeBlock
		{
			size_t size; //!< Size of the free block
			FreeBlock* next; //!< A pointer to the next free block
		};

		FreeListAllocator(const FreeListAllocator&) = delete; //!< prevent copies to avoid errors
		FreeListAllocator& operator=(const FreeListAllocator&) = delete; //!< prevent copies to avoid errors

		FreeBlock* free_blocks_; //!< A pointer to the first free block in the allocator
	};
}