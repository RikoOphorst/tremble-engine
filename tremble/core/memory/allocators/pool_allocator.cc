#include "pool_allocator.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	PoolAllocator::PoolAllocator(size_t size, size_t object_size, u8 alignment, void* start)
		:Allocator(size, start), object_size_(object_size)
	{
		ASSERT(object_size >= sizeof(void*));

		//Calculate adjustment needed to make the first object correctly aligned
		u8 adjustment = pointer_math::AlignForwardAdjustment(start, alignment);

		//Make free list point to the start of the memory with the objects in it
		free_list_ = (void**)pointer_math::Add(start, adjustment);

		//number of objects, that are able to fit inside the allocator
		size_t num_objects = (size - adjustment) / object_size;

		//Make p point to the start of the memory with the objects in it
		void** p = free_list_;

		//Initialize all objects(Make all objects to contain the memory address to the next object)
		for (size_t i = 0; i < num_objects - 1; i++)
		{
			//make the pointer, to which p points(currently initialized object) to point to the next object
			*p = pointer_math::Add(p, object_size);
			//move over the local p variable to point to the next object
			p = (void**)*p;
		}
		
		*p = nullptr;
	}

	//------------------------------------------------------------------------------------------------------
	PoolAllocator::~PoolAllocator()
	{
		free_list_ = nullptr;
	}

	//------------------------------------------------------------------------------------------------------
	void* PoolAllocator::Allocate(size_t size, u8 alignment)
	{
        allocator_lock_.lock();
		//Only object size, that is specified in the constructor can be used
		ASSERT(size == object_size_);

		//No free space left in the allocator
		if (free_list_ == nullptr)
		{
			return nullptr;
		}

		//return the pointer to the next free piece of memory in the allocator
		void* p = free_list_;

		//Make free list point to the next free piece of memory(Make it be the pointer, to which it pointed pefore)
		free_list_ = (void**)(*free_list_);

		used_memory_ += size;
		num_allocations_++;

        allocator_lock_.unlock();
		return p;
	}

	//------------------------------------------------------------------------------------------------------
	void PoolAllocator::Deallocate(void* p)
	{
        allocator_lock_.lock();
		//Make the memory address, with the object that is being deallocated point to the next free piece of space in the allocator
		*((void**)p) = free_list_;

		//Make the memory address of the currently deallocated object be the new first free piece of space in the allocator
		free_list_ = (void**)p;

		used_memory_ -= object_size_;
		num_allocations_--;
        allocator_lock_.unlock();
	}
}