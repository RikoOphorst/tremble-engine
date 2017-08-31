#pragma once
#include "allocator.h"

namespace tremble
{
	/**
	* @class tremble::ProxyAllocator
	* @brief Proxy allocator class. Used only for debugging purposes
	*
	* Proxy alloactor simply redirects allocations to the allocator, stored inside, and tracks memory usage of that allocator
	* @author Anton Gavrilov
	*/
	class ProxyAllocator : public Allocator
	{
	public:
		/*
		* @brief Proxy allocator constructor.
		* @param allocator Constructed allocator, of which do you want to track the memory usage
		*/
		ProxyAllocator(Allocator& allocator);
		~ProxyAllocator(); //!< Proxy allocator destructor

		/**
		* @brief Allocate some memory inside of the allocator, stored inside of proxy allocator
		* @param size Size of memory, that you want to allocate
		* @param alignment Alignment, that you wish this memory to have
		* @return raw pointer to the memory address, where the memory was allocated. If there is not enough memory in the allocator, returns 0
		*/
		void* Allocate(size_t size, u8 alignment) override;

		/**
		* @brief Deallocate a piece of memory inside of the internal allocator
		* @param p Pointer to memory, that you wish to deallocate
		*/
		void Deallocate(void* p) override;
	private:
		ProxyAllocator(const ProxyAllocator&) = delete; //!< Prevent copies to avoid errors
		ProxyAllocator& operator=(const ProxyAllocator&) = delete; //!< Prevent copies to avoid errors

		Allocator& allocator_; //!< internal allocator, through which all the allocations and deallocations take place
	};

	namespace allocator
	{
		/**
		* @brief Create a new Proxy allocator with the provided allocator
		* @param allocator Allocator, which you want to track usage of
		*/
		inline ProxyAllocator* newProxyAllocator(Allocator& allocator)
		{
			void* p = allocator.Allocate(sizeof(ProxyAllocator), __alignof(ProxyAllocator));
			return new (p)ProxyAllocator(allocator);
		}
	}
}