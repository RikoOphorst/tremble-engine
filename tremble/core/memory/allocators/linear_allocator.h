#pragma once
#include "allocator.h"
#include "../../utilities/utilities.h"

namespace tremble
{
	/**
	* @class tremble::LinearAllocator
	* @brief A linear Allocator class. The simplest and fastest allocator out of all. 
	* 
	* Memory is allocated consequentionally. Deallocated only with the deletion of the whole allocator
	* @author Anton Gavrilov
	*/
	class LinearAllocator : public Allocator
	{
	public:
		/**
		* @brief Linear allocator constructor
		* @param size Size of memory, reserved for this allocator
		* @param start Pointer to the memory at the start of the allocator
		*/
		LinearAllocator(size_t size, void* start);
		~LinearAllocator(); //!< Linear allocator destructor

		/**
		* @brief Allocate some memory inside this linear allocator
		* @param size Size of the memory, that you want to allocate
		* @param alingment Wished alingment of the memory. 4 by default
		* @return raw pointer to the memory address, where the memory was allocated. If there is not enough memory in the allocator, returns 0
		*/
		void* Allocate(size_t size, u8 alignment = 4) override;

		void Deallocate(void* p) override; //!< Deallocation of an object is impossible with the linear allocator. Use clear() instead

		void Clear();//!< Reset the allocator
	private:
		LinearAllocator(const LinearAllocator&) = delete; //!< Prevent copies to avoid errors
		LinearAllocator& operator=(const LinearAllocator&) = delete; //!< Prevent copies to avoid errors

		void* current_pos_; //!< Pddress of the start of unused memory in this allocator
	};
}