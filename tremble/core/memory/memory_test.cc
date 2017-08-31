#include "memory_test.h"
#include <vector>

#define LINEAR_ALLOC 1
#define STACK_ALLOC 1
#define FREELIST_ALLOC 1
#define POOL_ALLOC 1

#define MAX_NUM_ALLOCS 100000

#define NUM_16B_ALLOCS 10000
#define NUM_256B_ALLOCS 1000
#define NUM_2MB_ALLOCS 50

#define MB2INBYTES  2097152 //2mb

#define MEM_SIZE 1048576000 //1GB

namespace tremble
{
	void TimeTest()
	{
		void* memory = malloc(MEM_SIZE);

		u64 ticks_per_second;
		u64 start;
		u64 end;

		double elapsed;

		void* allocs[MAX_NUM_ALLOCS];

		QueryPerformanceFrequency((LARGE_INTEGER*)&ticks_per_second);

#if LINEAR_ALLOC
		/////////////////////////////////////////////////////////////////
		//Linear
		/////////////////////////////////////////////////////////////////
        
		LinearAllocator* linear_allocator = new LinearAllocator(MEM_SIZE, memory);

		printf("----------------\n");
		printf("Malloc vs Linear\n");
		printf("----------------\n");

		QueryPerformanceCounter((LARGE_INTEGER*)&start);

		int k = 0;

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			allocs[k + i] = malloc(16);

		k += NUM_16B_ALLOCS;

		for (int i = 0; i < NUM_256B_ALLOCS; i++)
			allocs[k + i] = malloc(256);

		k += NUM_256B_ALLOCS;

		for (int i = 0; i < NUM_2MB_ALLOCS; i++)
			allocs[k + i] = malloc(MB2INBYTES);

		k += NUM_2MB_ALLOCS;

		for (int i = 0; i < k; i++)
			free(allocs[i]);

		QueryPerformanceCounter((LARGE_INTEGER*)&end);

		elapsed = (end - start) / (double)ticks_per_second;

		printf("Malloc: %f\n", elapsed);

		QueryPerformanceCounter((LARGE_INTEGER*)&start);

		k = 0;

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			allocs[k + i] = linear_allocator->Allocate(16, 8);

		k += NUM_16B_ALLOCS;

		for (int i = 0; i < NUM_256B_ALLOCS; i++)
			allocs[k + i] = linear_allocator->Allocate(256, 8);

		k += NUM_256B_ALLOCS;

		for (int i = 0; i < NUM_2MB_ALLOCS; i++)
			allocs[k + i] = linear_allocator->Allocate(MB2INBYTES, 8);

		k += NUM_2MB_ALLOCS;

		linear_allocator->Clear();

		QueryPerformanceCounter((LARGE_INTEGER*)&end);

		elapsed = (end - start) / (double)ticks_per_second;

		printf("Linear: %f\n\n", elapsed);

		delete linear_allocator;
#endif

		/////////////////////////////////////////////////////////////////
		//Stack
		/////////////////////////////////////////////////////////////////
#ifdef STACK_ALLOC

		StackAllocator* stack_allocator = new StackAllocator(MEM_SIZE, memory);

		printf("----------------\n");
		printf("Malloc vs Stack\n");
		printf("----------------\n");

		QueryPerformanceCounter((LARGE_INTEGER*)&start);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			allocs[i] = malloc(16);

		for (int i = NUM_16B_ALLOCS - 1; i >= 0; i--)
			free(allocs[i]);

		for (int i = 0; i < NUM_256B_ALLOCS; i++)
			allocs[i] = malloc(256);

		for (int i = NUM_256B_ALLOCS - 1; i >= 0; i--)
			free(allocs[i]);

		for (int i = 0; i < NUM_2MB_ALLOCS; i++)
			allocs[i] = malloc(MB2INBYTES);

		for (int i = NUM_2MB_ALLOCS - 1; i >= 0; i--)
			free(allocs[i]);

		QueryPerformanceCounter((LARGE_INTEGER*)&end);

		elapsed = (end - start) / (double)ticks_per_second;

		printf("Malloc: %f\n", elapsed);

		QueryPerformanceCounter((LARGE_INTEGER*)&start);

		allocs[0] = stack_allocator->Allocate(16, 8);
		allocs[1] = stack_allocator->Allocate(16, 8);
		allocs[2] = stack_allocator->Allocate(16, 8);
		allocs[3] = stack_allocator->Allocate(16, 8);
		stack_allocator->Deallocate(allocs[3]);
		stack_allocator->Deallocate(allocs[2]);
		stack_allocator->Deallocate(allocs[1]);
		stack_allocator->Deallocate(allocs[0]);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			allocs[i] = stack_allocator->Allocate(16, 8);

		for (int i = NUM_16B_ALLOCS - 1; i >= 0; i--)
			stack_allocator->Deallocate(allocs[i]);

		for (int i = 0; i < NUM_256B_ALLOCS; i++)
			allocs[i] = stack_allocator->Allocate(256, 8);

		for (int i = NUM_256B_ALLOCS - 1; i >= 0; i--)
			stack_allocator->Deallocate(allocs[i]);

		for (int i = 0; i < NUM_2MB_ALLOCS; i++)
			allocs[i] = stack_allocator->Allocate(MB2INBYTES, 8);;

		for (int i = NUM_2MB_ALLOCS - 1; i >= 0; i--)
			stack_allocator->Deallocate(allocs[i]);

		QueryPerformanceCounter((LARGE_INTEGER*)&end);

		elapsed = (end - start) / (double)ticks_per_second;

		printf("Stack: %f\n\n", elapsed);

		delete stack_allocator;
#endif

#if FREELIST_ALLOC

		/////////////////////////////////////////////////////////////////
		//FreeList
		/////////////////////////////////////////////////////////////////

		FreeListAllocator* free_list_allocator = new FreeListAllocator(MEM_SIZE, memory);

		printf("------------------\n");
		printf("Malloc vs FreeList\n");
		printf("------------------\n");

		QueryPerformanceCounter((LARGE_INTEGER*)&start);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			allocs[i] = malloc(16);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			free(allocs[i]);

		for (int i = 0; i < NUM_256B_ALLOCS; i++)
			allocs[i] = malloc(256);

		for (int i = 0; i < NUM_256B_ALLOCS; i++)
			free(allocs[i]);

		for (int i = 0; i < NUM_2MB_ALLOCS; i++)
			allocs[i] = malloc(MB2INBYTES);

		for (int i = 0; i < NUM_2MB_ALLOCS; i++)
			free(allocs[i]);

		QueryPerformanceCounter((LARGE_INTEGER*)&end);

		elapsed = (end - start) / (double)ticks_per_second;

		printf("Malloc: %f\n", elapsed);

		QueryPerformanceCounter((LARGE_INTEGER*)&start);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			allocs[i] = free_list_allocator->Allocate(16, 8);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			free_list_allocator->Deallocate(allocs[i]);

		for (int i = 0; i < NUM_256B_ALLOCS; i++)
			allocs[i] = free_list_allocator->Allocate(256, 8);

		for (int i = 0; i < NUM_256B_ALLOCS; i++)
			free_list_allocator->Deallocate(allocs[i]);

		for (int i = 0; i < NUM_2MB_ALLOCS; i++)
			allocs[i] = free_list_allocator->Allocate(MB2INBYTES, 8);

		for (int i = 0; i < NUM_2MB_ALLOCS; i++)
			free_list_allocator->Deallocate(allocs[i]);

		QueryPerformanceCounter((LARGE_INTEGER*)&end);

		elapsed = (end - start) / (double)ticks_per_second;

		printf("FreeList: %f\n\n", elapsed);

		delete free_list_allocator;

#endif

#if POOL_ALLOC

		/////////////////////////////////////////////////////////////////
		//Pool
		/////////////////////////////////////////////////////////////////

		PoolAllocator* pool_allocator = new PoolAllocator(16, 8, MEM_SIZE, memory);

		printf("------------------\n");
		printf("Malloc vs Pool\n");
		printf("------------------\n");

		QueryPerformanceCounter((LARGE_INTEGER*)&start);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			allocs[i] = malloc(16);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			free(allocs[i]);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			allocs[i] = malloc(16);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			free(allocs[i]);

		QueryPerformanceCounter((LARGE_INTEGER*)&end);

		elapsed = (end - start) / (double)ticks_per_second;

		printf("Malloc: %f\n", elapsed);

		QueryPerformanceCounter((LARGE_INTEGER*)&start);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			allocs[i] = pool_allocator->Allocate(16, 8);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			pool_allocator->Deallocate(allocs[i]);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			allocs[i] = pool_allocator->Allocate(16, 8);

		for (int i = 0; i < NUM_16B_ALLOCS; i++)
			pool_allocator->Deallocate(allocs[i]);

		QueryPerformanceCounter((LARGE_INTEGER*)&end);

		elapsed = (end - start) / (double)ticks_per_second;

		printf("Pool: %f\n\n", elapsed);

		delete pool_allocator;
#endif
		free(memory);
	}

	void FreeListTest()
	{
		char* bigbuf = (char*)malloc(10000);

		// remember our allocs
		std::vector<void*> allocs_;

		FreeListAllocator allocator(10000, bigbuf);

		// lots of iterations
		for (int i = 0; i < 1000; i++)
		{
			// allocate blocks till the allocator is full
			for (;;)
			{
				size_t allocator_size = rand() % 500 + 1;
				u8 alignment = rand() % 2 == 0 ? 4 : 2;

				//printf("%d %d\n", (int) allocSize, alignment);

				void* alloc = allocator.Allocate(allocator_size, alignment);

				if (!alloc)
					break;

				allocs_.push_back(alloc);
			}

			if (i % 2 == 0)
			{
				// randomly deallocate ~50% of blocks (forward)
				for (size_t j = 0; j < allocs_.size(); j++)
				{
					if ((rand() % 2) == 0 && allocs_[j] != nullptr)
					{
						allocator.Deallocate(allocs_[j]);
						allocs_[j] = nullptr;
					}
				}
			}
			else
			{
				// randomly deallocate ~50% of blocks (backwards)
				for (int j = static_cast<int>(allocs_.size()) - 1; j >= 0; j--)
				{
					if ((rand() % 2) == 0 && allocs_[j] != nullptr)
					{
						allocator.Deallocate(allocs_[j]);
						allocs_[j] = nullptr;
					}
				}
			}
		}

		for (size_t i = 0; i < allocs_.size(); i++)
		{
			if (allocs_[i] != nullptr)
			{
				allocator.Deallocate(allocs_[i]);
				allocs_[i] = nullptr;
			}
		}

		printf("%d == %d\n", 0, (int)allocator.GetUsedMemory());

		getchar();

		return;
	}
}