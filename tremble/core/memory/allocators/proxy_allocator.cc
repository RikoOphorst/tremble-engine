#include "proxy_allocator.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	ProxyAllocator::ProxyAllocator(Allocator& allocator)
		:Allocator(allocator.GetSize(), allocator.GetStart()), allocator_(allocator)
	{

	}

	//------------------------------------------------------------------------------------------------------
	ProxyAllocator::~ProxyAllocator()
	{

	}

	//------------------------------------------------------------------------------------------------------
	void* ProxyAllocator::Allocate(size_t size, u8 alignment)
	{
        allocator_lock_.lock();
		ASSERT(size != 0);
		num_allocations_++;

		size_t mem = allocator_.GetUsedMemory();

		void* p = allocator_.Allocate(size, alignment);

		used_memory_ += allocator_.GetUsedMemory() - mem;

        allocator_lock_.unlock();
		return p;
	}

	//------------------------------------------------------------------------------------------------------
	void ProxyAllocator::Deallocate(void* p)
	{
        allocator_lock_.lock();
		num_allocations_--;

		size_t mem = allocator_.GetUsedMemory();

		allocator_.Deallocate(p);

		used_memory_ -= mem - allocator_.GetUsedMemory();
        allocator_lock_.unlock();
	}
}