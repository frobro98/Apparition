// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Memory/Internal/MemoryBlockInfoTable.hpp"
#include "Memory/Internal/MemoryFixedBlock.hpp"
#include "Memory/MemoryCore.hpp"
#include "Threading/CriticalSection.hpp"
#include "Platform/PlatformMemory.hpp"
#include "Threading/ScopedLock.hpp"

// TODO - This will be replaced with logging
#include "Debugging/DebugOutput.hpp"

namespace Memory::Internal
{
extern bool isInitialized;

// Initialize Memory Core first thing
//#pragma warning(disable:4075)
//#pragma init_seg(".CRT$XCT")

// TODO - This probably wants to be somewhere else because then we can have a better system around logging metrics
struct MemoryStats
{
	size_t allocatedFixedMemory = 0;
	size_t allocatedBigMemory = 0;
	size_t usedFixedMemory = 0;
	size_t usedBigMemory = 0;
};
static MemoryStats memoryStats;

void InitializeFixedBlockTable();
void InitializeMemoryInfoTable();

u8 FixedSizeToTableIndex(size_t alignedSize);
FixedBlockTableElement& GetFixedBlockInternal(u8 index);

//////////////////////////////////////////////////////////////////////////
// Memory Block Info Accessors
//////////////////////////////////////////////////////////////////////////
MemoryBlockInfo& InitializeOrFindMemoryInfo(void* p, BlockType blockType);
void DeinitializeMemoryInfo(MemoryBlockInfo& info);
MemoryBlockInfo* FindExistingMemoryInfo(void* p);

//////////////////////////////////////////////////////////////////////////
// Malloc Functions
//////////////////////////////////////////////////////////////////////////
forceinline void* MallocFixedBlock(size_t size)
{
	// Do fixed block allocation
	const u8 tableIndex = FixedSizeToTableIndex(size);//*(fixedSizeToIdxCachePtr+500);//fixedSizeToIndexCache[500];//FixedSizeToTableIndex(size);

	// TODO - Critical Section per table element
	FixedBlockTableElement& tableElement = GetFixedBlockInternal(tableIndex);
	
	ScopedLock poolLock(tableElement.critSection);

	FixedBlockPool* pool = tableElement.availablePools;
	if (pool == nullptr)
	{
		// Create pool and link it to front and back
		pool = AllocateNewPoolFor(tableElement, tableIndex);
		memoryStats.allocatedFixedMemory += PageAllocationSize;

		MemoryBlockInfo& blockInfo = InitializeOrFindMemoryInfo(pool->nextFreeBlock, BlockType::FixedSmallBlock);
		blockInfo.allocatedSize = tableElement.fixedBlockSize;
	}
	Assert(pool);

	// Allocate block from fixed block pool
	void* p = AllocateFromFreedBlock(*pool);
	if (pool->blocksInUse == tableElement.blocksPerPage)
	{
		tableElement.availablePools = pool->next;
		pool->next = tableElement.emptyPools;
		pool->prev = nullptr;
		if (tableElement.emptyPools)
		{
			tableElement.emptyPools->prev = pool;
		}
		tableElement.emptyPools = pool;
	}
	memoryStats.usedFixedMemory += tableElement.fixedBlockSize;
	return p;
}

forceinline void* MallocLargeBlock(size_t size, size_t alignment)
{
	size_t alignedSize = Align(size, alignment);
	// Do big boi allocation from OS
	void* ret = PlatformMemory::PlatformAlloc(alignedSize);
	Assert(ret);
	MemoryBlockInfo& blockInfo = InitializeOrFindMemoryInfo(ret, BlockType::FixedSmallBlock);
	blockInfo.allocatedSize = alignedSize;

	memoryStats.allocatedBigMemory += alignedSize;
	memoryStats.usedBigMemory += alignedSize;

	return ret;
}

//////////////////////////////////////////////////////////////////////////
// Free Functions
//////////////////////////////////////////////////////////////////////////

// TODO - Cache a certain amount of pages to prevent OS free calls happening a bunch
forceinline void FreeFixedBlock(void* p)
{
	// We know now that this is a fixed block allocation. We need to get back to the main "FreedBlock" header
	FreedBlock* fixedBlockHeader = GetFixedBlockHeader(p);
	Assert(fixedBlockHeader->headerID == FreedBlock::BlockTag);
	u8 tableIndex = FixedSizeToTableIndex(fixedBlockHeader->blockSize);
	FixedBlockTableElement& tableElement = GetFixedBlockInternal(tableIndex);

	FreedBlock* freeBlock = (FreedBlock*)p;
#if M_DEBUG
	Memfill(freeBlock, 0xdeaddead, fixedBlockHeader->blockSize);
#endif
	freeBlock->blockSize = fixedBlockHeader->blockSize;
	freeBlock->headerID = FreedBlock::BlockTag;
	freeBlock->numFreeBlocks = 1;
	freeBlock->nextBlock = nullptr;

	ScopedLock poolLock(tableElement.critSection);
	FixedBlockPool* pool = PushFreeBlockToPool(tableElement, freeBlock);
	Assert(pool);
	memoryStats.usedFixedMemory -= tableElement.fixedBlockSize;

	// If pool was previously empty, then move it to available list
	if (pool->blocksInUse == (tableElement.blocksPerPage - 1))
	{
		if (pool->prev)
		{
			pool->prev->next = pool->next;
		}
		if (pool->next)
		{
			pool->next->prev = pool->prev;
		}
		if (pool == tableElement.emptyPools)
		{
			tableElement.emptyPools = pool->next;
		}
		pool->next = tableElement.availablePools;
		pool->prev = nullptr;
		if (tableElement.availablePools)
		{
			tableElement.availablePools->prev = pool;
		}
		tableElement.availablePools = pool;
	}
	// If pool doesn't have any blocks in use, free it up baby
	else if (pool->blocksInUse == 0)
	{
#if M_DEBUG
		Memfill(fixedBlockHeader, 0xdeaddead, PageAllocationSize);
#endif

		PlatformMemory::PlatformFree(fixedBlockHeader);
		memoryStats.allocatedFixedMemory -= PageAllocationSize;

		// Reset blockInfo
		MemoryBlockInfo* blockInfo = FindExistingMemoryInfo(p);
		Assert(blockInfo);
		DeinitializeMemoryInfo(*blockInfo);

		// Unlink pool from list of pools
		if (pool->prev)
		{
			pool->prev->next = pool->next;
		}
		if (pool->next)
		{
			pool->next->prev = pool->prev;
		}
		if (pool == tableElement.availablePools)
		{
			tableElement.availablePools = pool->next;
		}

		poolManager.ReturnPoolNode(*pool);
	}
}

forceinline void FreeLargeBlock(void* p)
{
	// Get block information
	MemoryBlockInfo* blockInfo = FindExistingMemoryInfo(p);
	Assert(blockInfo);

	PlatformMemory::PlatformFree(p);

	memoryStats.allocatedBigMemory -= blockInfo->allocatedSize;
	memoryStats.usedBigMemory -= blockInfo->allocatedSize;

	DeinitializeMemoryInfo(*blockInfo);
}
}
