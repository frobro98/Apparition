// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Platform/PlatformMemory.hpp"
#include "Memory/MemoryDefinitions.hpp"
#include "Memory/MemoryFunctions.hpp"
#include "Memory/Internal/MemoryInternalDefinitions.hpp"
#include "Threading/CriticalSection.hpp"
#include "Utilities/Array.hpp"

namespace Memory::Internal
{

static forceinline bool ShouldAdjustBlockCountForHeader(u16 numFreeBlocks, u16 blockSize)
{
	return (numFreeBlocks * blockSize + AllocationDefaultAlignment) > PageAllocationSize;
}

static forceinline u16 GetAdjustedFreeBlocksFor(u16 blockSize)
{
	u16 numFreeBlocks = (u16)(PageAllocationSize / blockSize);
	if (ShouldAdjustBlockCountForHeader(numFreeBlocks, blockSize))
	{
		--numFreeBlocks;
	}
	return numFreeBlocks;
}
// Header of the entire page of fixed block memory
// Must be 16 bytes because it needs to fit into the smallest block size
struct FreedBlock
{
	static constexpr u8 BlockTag = 0xfe;

	FreedBlock(u16 blockSize_, u8 tableIndex_)
		: tableIndex(tableIndex_),
		blockSize(blockSize_)
	{
		numFreeBlocks = GetAdjustedFreeBlocksFor(blockSize);
	}

	// This block is a pointer to the next block outside of the contiguous memory within this block
	FreedBlock* nextBlock = nullptr;
	u8 tableIndex;
	u8 headerID = BlockTag;
	u16 blockSize;
	u16 numFreeBlocks;
};
static_assert(sizeof(FreedBlock) == 16);

// Where does this live???
// Could live in another page of memory
// What happens when that memory is exhausted??
// Can't realloc the memory because that destroys all pointers to the data
// Could have some sort of manager structure that keeps track of some sort of next/prev headers?
struct FixedBlockPool
{
	FixedBlockPool* next = nullptr;
	FixedBlockPool* prev = nullptr;
	// TODO - This is sort of hacky
	// If freed list is empty, store address to check against
	union
	{
		FreedBlock* nextFreeBlock = nullptr;
		uptr addr;
	};
	u16 blocksInUse = 0;
};
static_assert(sizeof(FixedBlockPool) == 32);

struct PoolPageHeader
{
	static constexpr u16 PoolPageHeaderFlag = 0xb00f;
	PoolPageHeader* next = nullptr;
	PoolPageHeader* prev = nullptr;
	FixedBlockPool* frontPoolNode = nullptr;
	u16 currentPoolHeaders = 0;
	u16 headerFlag = PoolPageHeaderFlag;
};
static_assert(sizeof(PoolPageHeader) == 32);

static forceinline PoolPageHeader* GetPoolPageHeader(void* p)
{
	return (PoolPageHeader*)((uptr)p & OSAllocationBitMask);
}

//////////////////////////////////////////////////////////////////////////
// Pool Header Manager
//////////////////////////////////////////////////////////////////////////

struct PoolHeaderManager
{
	static constexpr u32 TotalPoolHeaders = (PageAllocationSize / sizeof(FixedBlockPool)) - 1; // Accounting for header

	static forceinline FixedBlockPool* GetNewPoolFromHeader(PoolPageHeader& header)
	{
		Assert(header.currentPoolHeaders < TotalPoolHeaders);
		FixedBlockPool* pool = nullptr;
		++header.currentPoolHeaders;
		if (header.frontPoolNode != nullptr)
		{
			// Order is very important here....
			pool = header.frontPoolNode;
			header.frontPoolNode = header.frontPoolNode->next;
			pool->next = nullptr;
		}
		else
		{
			u8* ptr = (u8*)(&header) + PageAllocationSize
				- (header.currentPoolHeaders) * sizeof(FixedBlockPool);
			pool = new(ptr) FixedBlockPool; // Moving back to front of the pool chunks
		}

		return pool;
	}

	// Gets available pool for free block management
	forceinline FixedBlockPool* GetAvailablePoolNode()
	{
		PoolPageHeader* poolHeader = availablePoolPages;
		if (poolHeader == nullptr)
		{
			void* alloc = PlatformMemory::PlatformAlloc(PageAllocationSize);
			Assert(alloc);
			Memory::Memzero(alloc, PageAllocationSize);
			poolHeader = new(alloc) PoolPageHeader;
			availablePoolPages = poolHeader;
		}
		Assert(poolHeader);

		FixedBlockPool* pool = GetNewPoolFromHeader(*poolHeader);
		Assert(pool);

		if (poolHeader->currentPoolHeaders == TotalPoolHeaders)
		{
			availablePoolPages = availablePoolPages->next;
			availablePoolPages->prev = nullptr;

			poolHeader->next = emptyPoolPages;
			poolHeader->prev = nullptr;
			emptyPoolPages->prev = poolHeader;
			emptyPoolPages = poolHeader;
		}
		return pool;
	}

	forceinline void ReturnPoolNode(FixedBlockPool& pool)
	{
		REF_CHECK(pool);

		Memzero(&pool, sizeof(FixedBlockPool));

		PoolPageHeader* header = GetPoolPageHeader(&pool);
		Assert(header->headerFlag == PoolPageHeader::PoolPageHeaderFlag);

		// Put pool in list
		pool.next = header->frontPoolNode;
		header->frontPoolNode = &pool;
		--header->currentPoolHeaders;

		// Bring it back from empty to available
		if (header->currentPoolHeaders == (TotalPoolHeaders - 1))
		{
			if (header->prev)
			{
				header->prev->next = header->next;
			}
			if (header->next)
			{
				header->next->prev = header->prev;
			}

			header->next = availablePoolPages;
			header->prev = nullptr;
			availablePoolPages->prev = header;
			availablePoolPages = header;
		}
	}

	PoolPageHeader* availablePoolPages = nullptr;
	PoolPageHeader* emptyPoolPages = nullptr;
};

static PoolHeaderManager poolManager;

//////////////////////////////////////////////////////////////////////////
// Fixed Block Table
//////////////////////////////////////////////////////////////////////////
struct FixedBlockTableElement
{
	CriticalSection critSection;
	FixedBlockPool* availablePools = nullptr;
	FixedBlockPool* emptyPools = nullptr;
	size_t fixedBlockSize = 0;
	size_t blocksPerPage = 0;
};

constexpr u16 SmallFixedTableSizes[] = {
	16, 32, 48, 64, 80, 96, 112, 128, 160, 192,
	224, 256, 336, 448, 496, 512, 544, 576, 608, 640,
	672, 704, 736, 768, 800, 864, 928, 992, 1024, 1152,
	1280, 1408, 1536, 1664, 1792, 1920, 2048, 2304, 2560, 2816,
	3072, 3328, 3584, 3840, 4096, 4352, 4608, 5120, 5632, 6144,
	6656, 7680, 8704, 9728, 10752, 11776, 13824, 15872, 17920, 19968,
	22016, 24064, 28160, MaxFixedTableSize
};
static_assert(ArraySize(SmallFixedTableSizes) == TotalSmallFixedTableSizes);

// Cache coherency
constexpr u16 ReversedSmallFixedTableSizes[] = {
	MaxFixedTableSize, 28160, 24064, 22016, 19968, 17920, 15872, 13824, 11776, 10752,
	9728, 8704, 7680, 6656, 6144, 5632, 5120, 4608, 4352, 4096,
	3840, 3584, 3328, 3072, 2816, 2560, 2304, 2048, 1920, 1792,
	1664, 1536, 1408, 1280, 1152, 1024, 992, 928, 864, 800,
	768, 736, 704, 672, 640, 608, 576, 544, 512, 496,
	448, 336, 256, 224, 192, 160, 128, 112, 96, 80,
	64, 48, 32, 16
};
static_assert(ArraySize(ReversedSmallFixedTableSizes) == TotalSmallFixedTableSizes);

static_assert(sizeof(FreedBlock) <= SmallFixedTableSizes[0],
	"The freed block header needs to be able to fit inside of 16 bytes");

static forceinline bool ShouldUseFixedBlocks(size_t size, size_t alignment)
{
	return size <= MaxFixedTableSize && alignment <= AllocationDefaultAlignment;
}

static forceinline u16 TableIndexToFixedSize(u8 tableIndex)
{
	return ReversedSmallFixedTableSizes[TotalSmallFixedTableSizes - tableIndex - 1];
}

static forceinline void ValidatePool(NOT_USED FixedBlockPool* pool)
{
#if 0
	FreedBlock* block = pool->nextFreeBlock;
	while (block != nullptr)
	{
		if (block->nextBlock == nullptr)
		{
			Assert(IsAllocationFromOS(block));
			//Assert(block->numFreeBlocks > 0); // Acceptable because of addr/pointer union
		}
		else
		{
			Assert(block->numFreeBlocks == 1);
		}

		block = block->nextBlock;
	}
#endif // M_DEBUG
}

static forceinline FixedBlockPool* AllocateNewPoolFor(FixedBlockTableElement& tableElement, u8 tableIndex)
{
	FixedBlockPool* pool = poolManager.GetAvailablePoolNode();

	void* alloc = PlatformMemory::PlatformAlloc(PageAllocationSize);
	Assert(alloc);
	// TODO - Check for out of memory issues
	FreedBlock* block = new(alloc) FreedBlock((u16)tableElement.fixedBlockSize, tableIndex);
	pool->nextFreeBlock = block;
	pool->next = tableElement.availablePools;
	if (tableElement.availablePools)
	{
		tableElement.availablePools->prev = pool;
	}
	tableElement.availablePools = pool;

	return pool;
}

static forceinline void* AllocateFromFreedBlock(FixedBlockPool& pool)
{
	FreedBlock* block = pool.nextFreeBlock;
	void* ret;
	--block->numFreeBlocks;
	if (IsAllocationFromOS(block))
	{
		ret = (u8*)block + PageAllocationSize - (block->numFreeBlocks + 1) * block->blockSize;
	}
	else
	{
		Assert(block->numFreeBlocks == 0);
		ret = block;
	}

	if (block->numFreeBlocks == 0)
	{
		if (block->nextBlock != nullptr)
		{
			pool.nextFreeBlock = block->nextBlock;
		}
		else
		{
			pool.addr = reinterpret_cast<uptr>(block);
		}
	}

	ValidatePool(&pool);

	++pool.blocksInUse;
	return ret;
}

static forceinline FreedBlock* GetFixedBlockHeader(void* p)
{
	return (FreedBlock*)((uptr)p & OSAllocationBitMask);
}

static forceinline bool IsAddressWithinPool(FixedBlockPool& pool, void* p)
{
	Assert(p);
	// Can't store address start range within pool, so have to do this
	// This is probably faster than what I was doing before anyways
	uptr addr = reinterpret_cast<uptr>(p);
	uptr blockAddr = reinterpret_cast<uptr>(pool.nextFreeBlock);
	return (blockAddr & OSAllocationBitMask) == (addr & OSAllocationBitMask);
}

static forceinline FixedBlockPool* PushFreeBlockToPool(FixedBlockTableElement& tableElement, FreedBlock* block)
{
	Assert(block);
	Assert(block->numFreeBlocks == 1);

	bool found = false;
	FixedBlockPool* curr = tableElement.emptyPools;
	while (curr != nullptr)
	{
		if (IsAddressWithinPool(*curr, block))
		{
			block->nextBlock = curr->nextFreeBlock;
			curr->nextFreeBlock = block;
			found = true;
			break;
		}
		curr = curr->next;
	}

	if (!found)
	{
		curr = tableElement.availablePools;
		while (curr != nullptr)
		{
			if (IsAddressWithinPool(*curr, block))
			{
				block->nextBlock = curr->nextFreeBlock;
				curr->nextFreeBlock = block;
				found = true;
				break;
			}
			curr = curr->next;
		}
	}
	Assert(found);

	ValidatePool(curr);

	Assert(curr->blocksInUse >= 1);
	--curr->blocksInUse;
	return curr;
}
}
