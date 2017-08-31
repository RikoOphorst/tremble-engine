#pragma once

#include "../../utilities/utilities.h"
#include <mutex>

namespace tremble
{
	/** 
	* @class tremble::Allocator
	* @brief A virtual allocator class, declaring all main allocator functions. Meant to be inherited by all the allocators
	*
	* Resource used for creation: http://www.gamedev.net/page/resources/_/technical/general-programming/c-custom-memory-allocation-r3010
	* @author Anton Gavrilov
	*/
	class Allocator
	{
	public:
		/**
		* @brief Allocator constructor
		* @param[in] size Memory size, used by this allocator
		* @param[in] start Pointer to the start of the memory, used by this allocator
		*/
		Allocator(size_t size, void* start)
		{
			start_ = start;
			size_ = size;
			used_memory_ = 0;
			num_allocations_ = 0;
		}

		/**
		* @brief Allocator destructor.
		*/
		virtual ~Allocator()
		{
			//make sure that this allocator is empty before deleting it
			ASSERT(num_allocations_ == 0 && used_memory_ == 0 && "Trying to deallocate an allocator that still holds some objects");
			start_ = nullptr;
			size_ = 0;
		}

		/**
		* @brief Allocate some memory inside this allocator
		* @param[in] size Size of the memory, that you want to allocate
		* @param[in] alingment Wished alingment of the memory. 4 by default
		* @return raw pointer to the memory address, where the memory was allocated. If there is not enough memory in the allocator, returns 0
		*/
		virtual void* Allocate(size_t size, u8 alignment = 4) = 0;

		/**
		* @brief Deallocate a piece of memory, start of which is located at p
		*/
		virtual void Deallocate(void* p) = 0;

		void* GetStart() const { return start_; } //!<Get start pointer of this allocator @return Start pointer of this allocator
		size_t GetSize() const { return size_; } //!<Get total size of this allocator @return Total size of this allocator
		size_t GetUsedMemory() const { return used_memory_; } //!<Get the size of used memory in this allocator @return The size of used memory in this allocator
		size_t GetNumAllocations() const { return num_allocations_; } //!<Get the number of allocations, made with this allocator @return The number of allocations, made with this allocator
		size_t GetFreeMemory() const { return size_ - used_memory_; }

		template <typename T, typename... Args>
		T* New(Args... args)
		{
			T* p = (T*)Allocate(sizeof(T), alignof(T));
			return new (p) T(args...);
		}

        template <typename T>
        T* New(T argument)
        {
            T* p = (T*)Allocate(sizeof(T), alignof(T));
            return new (p) T(argument);
        }

		template <typename T>
		std::enable_if_t <std::is_polymorphic<T>::value> Delete(T* p)
		{
			p->~T();
			Deallocate(static_cast<void*>(p));
		}

		template <typename T>
		std::enable_if_t <!std::is_polymorphic<T>::value> Delete(T* p)
		{
			p->~T();
			Deallocate(p);
		}

        /**
        * @brief Get a new allocator Inside of this allocator with the memory of the allocator next to it.
        * @param size Size, that you wish the allocator to have
        * @tparam AllocatorClass Class of the allocator that you want to get. Has to be a derived class from tremble::Allocator
        * @return A pointer tio the initialized allocator
        */
        template<typename AllocatorClass>
        AllocatorClass* NewAllocator(size_t size)
        {
            if (std::is_base_of<Allocator, AllocatorClass>() == true)
            {
                void* p = Allocate(sizeof(AllocatorClass) + size, alignof(AllocatorClass));
                return new (p) AllocatorClass(size, pointer_math::Add(p, sizeof(AllocatorClass)));
            }
            return nullptr;
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
        AllocatorClass* NewAllocator(size_t size, size_t object_size, u8 alignment)
        {
            if (std::is_base_of<PoolAllocator, AllocatorClass>() == true)
            {
                void* p = Allocate(sizeof(AllocatorClass) + size, alignof(AllocatorClass));
                return new (p) AllocatorClass(size, object_size, alignment, pointer_math::Add(p, sizeof(AllocatorClass)));
            }
            return nullptr;
        }

        /**
        * @brief Get a new pool allocator to manage a piece of memory for a subsystem of a game.
        * @param size Size, that you wish the allocator to have
        * @tparam Object object type, that the pool allocator will be used for storing
        * @tparam AllocatorClass Class of the allocator that you want to get. Has to be a derived class from engine::Allocator
        * @return A pointer tio the initialized allocator
        */
        template<typename AllocatorClass, typename Object>
        AllocatorClass* NewAllocator(size_t size)
        {
            if (std::is_base_of<PoolAllocator, AllocatorClass>() == true)
            {
                void* p = Allocate(sizeof(AllocatorClass) + size, alignof(AllocatorClass));
                return new (p) AllocatorClass(size, sizeof(Object), alignof(Object), pointer_math::Add(p, sizeof(AllocatorClass)));
            }
            return nullptr;
        }

        /**
        * @brief Remove an allocator
        * @param p A pointer to the alocator that you want to deallocate
        */
        template<typename AllocatorClass>
        void DeleteAllocator(AllocatorClass* p)
        {
            p->~AllocatorClass();
            Deallocate(p);
        }

	protected:
		void* start_; //!< Raw pointer to the start of memory of this allocator
		size_t size_; //!< Size of the memory, taken up by this allocator
		size_t used_memory_; //!< Size of used memory inside of this allocator
		size_t num_allocations_; //!< Number of allocations, made with this allocator
        std::mutex allocator_lock_; //!< Used to lock the allocator so that other threads cannot use it
	};

	namespace allocator
	{
		/**
		* @brief Template function to allocate a new object with an allocator
		* @param[in] allocator An allocator that you wish to use for the allocation
		*/
		template <class T> T* AllocateNew(Allocator& allocator)
		{
			return new (allocator.Allocate(sizeof(T), __alignof(T))) T;
		}

		template <class T> T* AllocateNew(Allocator& allocator, const T& t)
		{
			return new (allocator.Allocate(sizeof(T), __alignof(T))) T(t);
		}

		/**
		* @brief Template function to destruct and deallocate an object in an allocator
		* @param[in] allocator An allocator, where the object is stored
		* @param object Object, that you wish to destruct, and then deallocate
		*/
		template <class T> void DeallocateDelete(Allocator& allocator, T& object)
		{
			object.~T();
			allocator.Deallocate(&object);
		}

		/** 
		* @brief Allocate array template function. Use whenever you want to allocate an array. (pretty self-explanatory isn't it)
		*
		* Allocates a large piece of memory inside the allocator to store an array. It stores a header with the size of the array before the array itself
		* @param allocator Allocator, which you wish to use for the allocation
		* @param length Length of the array, which is being allocated
		* @return A memory adress, where the array can be stored
		*/
		template<class T> T* AllocateArray(Allocator& allocator, size_t length)
		{
			ASSERT(length != 0);

			//actual header size is size_t. It just stores the amount of objects in the array
			u8 header_size = sizeof(size_t) / sizeof(T);

			if (sizeof(size_t) % sizeof(T) > 0)
				header_size += 1;

			T* p = ((T*)allocator.Allocate(sizeof(T)*(length + header_size), __alignof(T)) + header_size);

			*(((size_t*)p) - 1) = length;

			for (size_t i = 0; i < length; i++)
			{
				new (&p[i]) T;
			}
			return p;
		}

		/**
		* @brief Template function to deallocate an array
		* @param allocator Allocator, where the array is stored
		* @param arr Array's memory address
		*/
		template <class T> void DeallocateArray(Allocator& allocator, T* arr)
		{
			ASSERT(arr != nullptr);

			size_t length = *(((size_t*)arr) - 1);

			for (size_t i = 0; i < length; i++)
			{
				arr[i].~T();
			}

			u8 header_size = sizeof(size_t) / sizeof(T);

			if (sizeof(size_t) % sizeof(T) > 0)
			{
				header_size += 1;
			}

			allocator.Deallocate(array - header_size);
		}
	}

	namespace pointer_math
	{
		/**
		* @brief Align an address forward to a certain alignment
		* @param[in] alignment Alignment, that you want to use for alignment of this address
		* @param[in] address Adress in memory, that you want to align
		* @return Aligned address
		*/
		inline void* AlignForward(void* address, u8 alignment)
		{
			//to align a memory address we mask off the least significant bits from the adress 
			
			return (void*)((reinterpret_cast<uptr>(address) + static_cast<uptr>(alignment - 1)) & static_cast<uptr>(~(alignment - 1)));
		}

		/**
		* @brief Calculate adjustment, needed for a specified alignment
		* @param[in] alignment Alignment, that you want to use for alignment of this address
		* @param[in] address Adress in memory, that you want to align
		* @return Adjustment, required to align the address
		*/
		inline u8 AlignForwardAdjustment(const void* address, u8 alignment)
		{
			u8 adjustment = alignment - (reinterpret_cast<uptr>(address) & static_cast<uptr>(alignment - 1));

			if (adjustment == alignment)
				return 0; //already aligned

			return adjustment;
		}

		/**
		* @brief Calculate adjustment, needed for a specified alignment, keeping in mind space for the header
		* @param alignment Alignment, that you want to use for alignment of this address
		* @param address Adress in memory, that you want to align
		* @param header_size Size of a header, memory for which is supposed to be allocated before the aligned address 
		* @return Adjustment, required to align the address and header
		*/
		inline u8 AlignForwardAdjustmentWithHeader(const void* address, u8 alignment, u8 header_size)
		{
			u8 adjustment = AlignForwardAdjustment(address, alignment);

			u8 neededSpace = header_size;

			//if header doesn't fit in the adjustment already
			if (adjustment < neededSpace)
			{
				neededSpace -= adjustment;

				//Increase adjustment to fit header
				adjustment += alignment * (neededSpace / alignment);

				if (neededSpace % alignment > 0)
				{
					adjustment += alignment;
				}
			}
			return adjustment;
		}

		/**
		* @brief Add a size to a pointer
		*
		* Used for memory addresses calculations. Example: If element b is located after element a, where would it be located, if the address of a is p, and size of a is x
		* @param p pointer, that you wish to add size to
		* @param x size, that you wish to add to the pointer
		*/
		inline void* Add(void* p, size_t x)
		{
			return (void*)(reinterpret_cast<uptr>(p) + x);
		}

		/**
		* @brief Add a size to a pointer
		*
		* Used for memory addresses calculations. Example: If element b is located after element a, where would the start of element b be located, if the address of a is p, and size of a is x
		* @param p pointer, that you wish to add size to
		* @param x size, that you wish to add to the pointer
		* @see Substract()
		*/
		inline const void* Add(const void* p, size_t x)
		{
			return (const void*)(reinterpret_cast<uptr>(p) + x);
		}

		/**
		* @brief Substract a size from a pointer
		*
		* Used for memory addresses calculations. Example: If element b is located after element a, where would the start of element a be located, if the address of b is p, and size of a is x
		* @param p pointer, that you wish to add size to
		* @param x size, that you wish to add to the pointer
		* @see Add()
		*/
		inline void* Substract(void* p, size_t x)
		{
			return (void*)(reinterpret_cast<uptr>(p) - x);
		}

		/**
		* @brief Substract a size from a pointer
		*
		* Used for memory addresses calculations. Example: If element b is located after element a, where would element a be located, if the address of b is p, and size of a is x
		* @param p pointer, that you wish to add size to
		* @param x size, that you wish to add to the pointer
		* @see Add()
		*/
		inline const void* Substract(const void* p, size_t x)
		{
			return (const void*)(reinterpret_cast<uptr>(p) - x);
		}
	}
}