#include "memory_manager.h"

#define MEMORY_FOR_ALLOCATORS 10000

namespace tremble
{
	MemoryManager::MemoryManager(size_t memory, size_t frame_temp_memory)
		:memory_size_(memory)
	{
		//Allocate the amount of memory that is to be used by the memory manager
		memory_ = malloc(memory);
		//Manually allocate the all_allocators allocators
		size_t adjustment = pointer_math::AlignForwardAdjustment(memory_, alignof(FreeListAllocator));
		void* allocator_address = pointer_math::Add(memory_, adjustment);
		void* allocator_memory_address = pointer_math::Add(allocator_address, sizeof(FreeListAllocator));
		all_allocators_ = new (allocator_address) FreeListAllocator(memory - sizeof(FreeListAllocator) - adjustment, allocator_memory_address);
        linear_temp_allocator_ = all_allocators_->NewAllocator<LinearAllocator>(frame_temp_memory);
	}

	MemoryManager::~MemoryManager()
	{
        all_allocators_->DeleteAllocator(linear_temp_allocator_);
		all_allocators_->~FreeListAllocator();
		free(memory_);
	}
}
