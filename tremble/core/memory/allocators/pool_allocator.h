#pragma once
#include "allocator.h"

namespace tremble
{
	/**
	* @class tremble::PoolAllocator
	* @brief A Pool Allocator class
	*
	* A pool allocator class, as the name suggests, keeps a pool of objects. All objects have to be the same size. Size is specified during the allocator constructor.
	* A pool allocator is faster than a more general-purpose free list allocator, and should be used if possible
	* It is fast when it comes to allocation or deallocation, however it takes time to construct it. Construction time depends on the size of the allocator
	* 
	* @author Anton Gavrilov
	*/
	class PoolAllocator : public Allocator
	{
	public:

		/**
		* @brief Pool Allocator constructor
		* Construction time depends linearly on the amount of objects that can fit in the allocator
		* @param object_size size of the individual objects, meant to be stored in the allocator
		* @param alignment alignment of the first object in the allocator. Later adjustment depends on the size of the objects
		* @param size Size of the allocator
		* @param start Pointer to the memory at the start of the allocator
		*/
		PoolAllocator(size_t size, size_t object_size, u8 alignment, void* start);
		~PoolAllocator(); //!< Pool allocator destructor

		/**
		* @brief Allocate memory for one object inside of this free list allocator
		* @param size Size of the object, that you want to allocate(has to be the object size, defined during construction)
		* @param alingment Wished alingment of the memory. 4 by default
		* @return Raw pointer to the memory address of the allocated memory
		*/
		void* Allocate(size_t size, u8 alignment = 4) override;

		/**
		* @brief Deallocate an object
		* @param p Pointer to the piece of memory, that you wish to deallocate
		*/
		void Deallocate(void* p) override;
	private:
		PoolAllocator(const PoolAllocator&) = delete; //!< prevent copies to avoid errors
		PoolAllocator& operator=(const PoolAllocator&) = delete; //!< prevent copies to avoid errors

		size_t object_size_; //!< size of objects insie the allocator

		void** free_list_; //!< Points to the next free piece of memory
	};
}