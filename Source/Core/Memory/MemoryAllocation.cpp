// Copyright 2020, Nathan Blane

#include "MemoryAllocation.hpp"
#include "Memory.hpp"
#include "Internal/MemoryAllocationInternal.hpp"

#include "Containers/StaticArray.hpp"
#include "Debugging/Assertion.hpp"
#include "Debugging/DebugOutput.hpp"
// TODO - IsPowerOf2 shouldn't be in this header...
#include "Utilities/MemoryUtilities.hpp"
#include "Utilities/Array.hpp"
#include "Utilities/BitUtilities.hpp"

#include "Internal/MemoryAllocationInternal.hpp"

// "Small" allocations
// 16 bytes - 32kb allocations fit in here
// Cached size -> index into table conversion
// Small allocations probably will take up the most allocations (not the most memory)
//
// Large allocations coming directly from the OS
// There will be some kind of caching solution that keeps a specific amount of memory
// around from these large allocations
//
// Blocks of memory need to have some sort of header to them, but without actually having
// the memory contain the header. Block info would describe the actual size of the block,
// be associated with a block of allocated memory, and live in some other structure that 
// uses the ptr address as a unique id to access this info. If I use a page (64Kb) to 
//
// So how to split up the address (unique id) so that they fall into a set amount of elements of block info?
// By shifting address over 16 (2 ^ 16 is 65536 or page size), we get rid of the 0s that are there because of alignment
// and get a more workable number. The bits at the bottom of the address vary a fair amount between allocations vs the upper bits,
// so this could be the table elements within the hash table
//
// This algorithm uses the important parts of the address, as well as the entire memory address spectrum
// This is currently on my system, which has 16Gb memory (really a little less, but it uses 16Gb exactly)
// Max buckets for this amount of memory is 128 and max pools per 64k is 2048 (pool is 32 bytes).
// You get these numbers by using the total amount of 64Kb allocations a system can essentially have
//
// My system (16Gb) can have have 262,144 allocations of 64kb (17,179,849,184 / 65536).
// If I were to allocate 64Kb per hash table element, I could fit 2048 32 byte headers within that memory.
// Because of this, if I divide total possible page allocations by number of headers per element (262,144 / 2048)
// I get the amount of table elements that are needed, which is 128
//
// CURRENTLY, THE THEORY BELOW DOESN'T WORK... NEED TO THINK ABOUT THIS MORE. USING 1MB COULD OFFER BETTER RESULTS DOWN THE ROAD...
// MIGHT BE ABLE TO MAP THE INDEX RECEIVED BY THE PTR TO SOME OTHER STRUCTURE THAT DOESN'T TAKE UP A WHOLE MEGABYTE TO MAKE THIS WORK...
// If I were to up my size and allocate 1Mb (1,048,576), I would be able to do that 16,384 times
// Keeping my page allocation, and the header count of 2048, I would do 16,384 / 2048, which results in 8 table elements
//
// For not power of 2 memory spaces, needs to essentially go to the nearest bit bit above the total memory space

// TODO - Research Virtual memory and understand how it works and how to map a lot of virtual memory space to 
// physical memory and make it worth while

// TODO - Look into using a Best-Fit RedBlack tree to store medium allocations (Hacky talked about this idea in GDC talk)
// Might not be needed, but it might be good to look at stats of allocations

using namespace Memory::Internal;

#define USE_MALLOC 0

namespace Memory
{
void* Memory::Malloc(size_t size, size_t alignment)
{
#if USE_MALLOC
	return _aligned_malloc(size, alignment);
#else
	if (!isInitialized)
	{
		InitializeMemory();
	}

	Assert(isInitialized);
	Assert(IsPowerOf2(alignment));

	if (ShouldUseFixedBlocks(size, alignment))
	{
		return MallocFixedBlock(size);
	}
	else
	{
		return MallocLargeBlock(size, alignment);
	}
#endif 
}

// Checks the actual size of the memory within the ptrs current allocation
// If it can fit the new size within the current block, it just returns that ptr
// else it uses Malloc and Free to get a new allocation
void* Memory::Realloc(void* ptr, size_t size, size_t alignment)
{
#if USE_MALLOC
	return _aligned_realloc(ptr, size, alignment);
#else
	if (!isInitialized)
	{
		InitializeMemory();
	}

	Assert(isInitialized);
	Assert(IsPowerOf2(alignment));

	// TODO - Lots of branching. See if it can be less
	if (size > 0)
	{
		// Reallocing a fixed block
		// More likely this branch because more allocations are fixed block than not
		if (!IsAllocationFromOS(ptr))
		{
			FreedBlock* fixedHeader = GetFixedBlockHeader(ptr);
			Assert(fixedHeader->headerID == FreedBlock::BlockTag);

			// Need to check if the size can fit within this fixed block, or if it needs
			// to find a new block
			u8 tableIndex = fixedHeader->tableIndex;
			// Checks to see if block needs to be actually realloced
			if (size > fixedHeader->blockSize || alignment > AllocationDefaultAlignment || // Greater than this
				(tableIndex != 0 && size <= TableIndexToFixedSize(tableIndex - 1))) // Check if the allocation can fit into the previous size element
			{
				void* ret = Malloc(size, alignment);
				Memcpy(ret, ptr, size);
				Free(ptr);
				return ret;
			}
			else
			{
				return ptr;
			}
		}
		// Not null, but not a fixed block allocation
		else if (ptr)
		{
			// Maybe make a function that does this specific thing?
			MemoryBlockInfo* blockInfo = FindExistingMemoryInfo(ptr);
			Assert(blockInfo);
			size_t allocatedSize = blockInfo->allocatedSize;
			// Test to see if it can use fixed block, if allocation can't fit into the current allocation
			
			bool useFixed = ShouldUseFixedBlocks(size, alignment);
			if (useFixed || size > allocatedSize)
			{
				void* ret = nullptr;
				if (useFixed)
				{
					ret = MallocFixedBlock(size);
				}
				else
				{
					ret = MallocLargeBlock(size, alignment);
				}

				Memcpy(ret, ptr, allocatedSize);
				Free(ptr);
				return ret;
			}

			return ptr;
		}
		else
		{
			// if null, just call malloc. Nothing to realloc
			return Malloc(size, alignment);
		}
	}

	// Size is 0, so we free ptr
	Free(ptr);
	return nullptr;
#endif
}

void Memory::Free(void* p)
{
#if USE_MALLOC
	_aligned_free(p);
#else
	if (!isInitialized)
	{
		InitializeMemory();
	}

	Assert(isInitialized);

	if (!IsAllocationFromOS(p))
	{
		FreeFixedBlock(p);
	}
	// p should be non-null so we end up not doing anything for Free on a nullptr
	else if (p)
	{
		FreeLargeBlock(p);
	}
#endif 
}
}


