#pragma once
#include "allocator.h"

namespace tremble
{
	/**
	* @class tremble::StackAllocator
	* @brief Stack allocator. Works like a stack. First in last out structure.
	* @author Anton Gavrilov
	*/
	class StackAllocator : public Allocator
	{
	public:
		
		/** 
		* @brief Stack allocator constructor
		* @param size Size of memory, reserved for this allocator
		* @param start Pointer to the memory at the start of the allocator
		*/
		StackAllocator(size_t size, void* start);
		~StackAllocator(); //!< Stack allocator destructor
		
		/**
		* @brief Allocate some memory inside this stack allocator
		* @param size Size of the memory, that you want to allocate
		* @param alingment Wished alingment of the memory. 4 by default
		* @return Raw pointer to the memory address of the allocated memory
		*/
		void* Allocate(size_t size, u8 adjustment = 4) override;

		/**
		* @brief Deallocate a piece of memory
		* @param p Pointer to the piece of memory, that you wish to deallocate
		*/
		void Deallocate(void* p) override;

	private:
		StackAllocator(const StackAllocator&) = delete; //!< prevent copies to avoid errors
		StackAllocator& operator=(const StackAllocator&) = delete; //!< prevent copies to avoid errors

		/**
		* @struct AllocationHeader
		* @brief Allocation header for the stack allocator class
		*/
		struct AllocationHeader
		{
#if _DEBUG
			void* prev_address; //!< address of the previous allocated element in the stack allocator. Only exists in debug mode
#endif
			u8 adjustment; //!< size of this header + adjustment, that was required for alignment of the following element
		};

#if _DEBUG
		void* prev_position_; //!< position of the previous element in the stack allocator
#endif
		void* current_pos_; //!< address of the start of unused memory in this allocator
	};
}