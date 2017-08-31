#pragma once

//#include "allocators/allocator.h"
#include "allocators/free_list_allocator.h"
#include "allocators/linear_allocator.h"
//#include "allocators/pool_allocator.h"
//#include "allocators/proxy_allocator.h"
//#include "allocators/stack_allocator.h"

namespace tremble
{
	/**
	* @brief An overall memory manager class. Handles allocation of allocators in our game.
	*
	* A single memory manager is meant to exist in a process. Different systems can allocate an allocator with a big chunk of memory for itself.
	* That allocator then manages the memory for each system.
	* @author Anton Gavrilov
	*/
	class MemoryManager
	{
        friend class GameManager;

	public:
		MemoryManager(size_t memory, size_t frame_temp_memory); //!< @param memory Amount of memory, that you want this memory manager to have.
		~MemoryManager(); //!< Frees all the memory in the memory manager
        size_t GetFreeMemory() { return all_allocators_->GetFreeMemory(); } //!< Get free memory inside of the memory manager

        /**
        * @brief New a temporary object. THis should be used only to allocate some space for the time being without worrying about deallocation. Space gets allocated 
        * in a linear allocator, that gets cleared every frame
        */
        template<typename T, typename... Args>
        T* NewTemp(Args... args)
        {
            return linear_temp_allocator_->New<T>(args...);
        }

        template<typename T, typename... Args>
        T* GlobalNew(Args... args)
        {

        }

        /**
        * @brief Get a new pool allocator to manage a piece of memory for a subsystem of a game.
        * @param size Size, that you wish the allocator to have
        * @param object_size Size, that you wish the objects inside the allocator to have
        * @param alignment Alignment, that you wish the objects inside the allocator to have
        * @tparam AllocatorClass Class of the allocator that you want to get. Has to be a derived class from engine::Allocator
        * @return A pointer tio the initialized allocator
        */
        template<typename AllocatorClass>
        AllocatorClass* GetNewAllocator(size_t size, size_t object_size, u8 alignment)
        {
            return all_allocators_->NewAllocator<AllocatorClass>(size, object_size, alignment);
        }

        /**
        * @brief Get a new pool allocator to manage a piece of memory for a subsystem of a game.
        * @param size Size, that you wish the allocator to have
        * @tparam Object object type, that the pool allocator will be used for storing
        * @tparam AllocatorClass Class of the allocator that you want to get. Has to be a derived class from engine::Allocator
        * @return A pointer tio the initialized allocator
        */
        template<typename AllocatorClass, typename Object>
        AllocatorClass* GetNewAllocator(size_t size)
        {
            return all_allocators_->NewAllocator<typename AllocatorClass, typename Object>(size);
        }

		/**
		* @brief Get a new allocator to manage a piece of memory for a subsystem of a game.
		* @param size Size, that you wish the allocator to have
		* @tparam AllocatorClass Class of the allocator that you want to get. Has to be a derived class from tremble::Allocator
		* @return A pointer tio the initialized allocator
		*/
		template<typename AllocatorClass>
		AllocatorClass* GetNewAllocator(size_t size)
		{
            return all_allocators_->NewAllocator<AllocatorClass>(size);
		}


		/**
		* @brief Remove an allocator from the memory manager
		* @param p A pointer to the alocator that you want to deallocate
		*/
		template<typename AllocatorClass> 
		void DeleteAllocator(AllocatorClass* p)
		{
            all_allocators_->DeleteAllocator(p);
		}

	private:

        /**
        * @brief Clear the temporary frame allocator at the end of the frame. Meant to be used by the game manager only
        */
        void ClearTemp_()
        {
            linear_temp_allocator_->Clear();
        }

		FreeListAllocator* all_allocators_; //!< A pointer to the free list allocator that manages all the allocators 
        LinearAllocator* linear_temp_allocator_; //!< An allocator to do temporary allocations in the moment. Gets cleared every frame
		void* memory_; //!< A pointer to the memory, that is allocated for the memory manager
		size_t memory_size_; //!< The size of the memory, that is in use by the memory manager
	};
}