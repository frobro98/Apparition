// Copyright 2020, Nathan Blane

#include "MemoryAllocationInternal.hpp"


#include "Memory/Internal/MemoryInternalDefinitions.hpp"


namespace Memory::Internal
{
static FixedBlockTableElement fixedBlockTable[TotalSmallFixedTableSizes];
static u8 fixedSizeToIndexCache[FixedSizeCacheCount];
static MemoryBlockInfoBucket blockInfoHashTable[Memory::Internal::MemoryBucketCount];

void InitializeFixedBlockTable()
{
	// Initialize small alloc table, currently using 128 size
	for (u32 i = 0; i < TotalSmallFixedTableSizes; ++i)
	{
		fixedBlockTable[i].fixedBlockSize = SmallFixedTableSizes[i];
		fixedBlockTable[i].blocksPerPage = GetAdjustedFreeBlocksFor(SmallFixedTableSizes[i]);
	}

	// initialize size -> table element array
	u8 tableIndex = 0;
	for (u16 i = 0; i < FixedSizeCacheCount; ++i)
	{
		// All sizes that come in will be aligned to 16 bytes, which then puts them into one of the fixed table elements
		u16 fixedBlockSize = i << BitsForDefaultAlignment;
		while (fixedBlockTable[tableIndex].fixedBlockSize < fixedBlockSize)
		{
			++tableIndex;
		}
		Assert(tableIndex < TotalSmallFixedTableSizes);
		fixedSizeToIndexCache[i] = tableIndex;

		MUSA_DEBUG(MemoryLog, "fixedSizeToIndexCache[{}] = {}", i, tableIndex);
	}
}

void InitializeMemoryInfoTable()
{
	// Apparently, VirtualAlloc allocates virtual pages when asking for memory less, or even more, than a single page.
	// However, it'll only fuck with virtual address space. So if I want to reserve and commit 4K, I do below...
	//constexpr size_t bucketMemorySize = Align(memoryBucketCount * sizeof(MemoryBlockInfoBucket), KilobytesAsBytes(4));
	//blockInfoHashTable = (MemoryBlockInfoBucket*)PlatformMemory::PlatformAlloc(MemoryBucketCount * sizeof(MemoryBlockInfoBucket));
	//Assert(blockInfoHashTable);

	for (u32 i = 0; i < MemoryBucketCount; ++i)
	{
		new(&blockInfoHashTable[i]) MemoryBlockInfoBucket;
	}
}

FixedBlockTableElement& GetFixedBlockInternal(u8 index)
{
	return fixedBlockTable[index];
}

//////////////////////////////////////////////////////////////////////////
// Memory Block Info Accessors
//////////////////////////////////////////////////////////////////////////
MemoryBlockInfo& InitializeOrFindMemoryInfo(void* p, BlockType blockType)
{
	// TODO - This might need some sort of synchronization
	// First thoughts are that it doesn't because the addresses coming from the OS
	// are all unique, so there wouldn't be any kind of issue accessing the same bucket.
	//
	// If there are, in fact, possible collisions that could happen because of "similar" addresses,
	// That kind of thing needs to be addressed. Might need a lock for that kind of thing.
	// Consult Hacky's implementation of NetherRealm's lock free behaviors and even lock
	// locations
	Assert(p);
	uptr addr = reinterpret_cast<uptr>(p);
	uptr preprocessedAddr = addr >> BitsInPageAllocation;
	u64 blockIndex = preprocessedAddr & BlockInfoMask;
	u64 memoryBucketIndex = (preprocessedAddr >> BitsOfBlockInfoMask) & MemoryBucketMask;

	// TODO - Make this parallel arrays of some kind
	MemoryBlockInfoBucket& bucket = blockInfoHashTable[memoryBucketIndex];
	if (bucket.blockInfo == nullptr)
	{
		bucket.blockInfo = (MemoryBlockInfo*)PlatformMemory::PlatformAlloc(PageAllocationSize);
		Assert(bucket.blockInfo);
		for (u32 i = 0; i < BlockInfosPerBucket; ++i)
		{
			new(&bucket.blockInfo[i]) MemoryBlockInfo;
		}
	}

	MemoryBlockInfo& blockInfo = bucket.blockInfo[blockIndex];
	blockInfo.flag = OwnedBlockFlag::YesBlock;
	if (blockInfo.type == BlockType::Untyped)
	{
		blockInfo.type = blockType;
	}
	else
	{
		Assert(blockInfo.type == blockType);
	}

	return blockInfo;
}

void DeinitializeMemoryInfo(MemoryBlockInfo& info)
{
	info.allocatedSize = 0;
	info.flag = OwnedBlockFlag::NoBlock;
	info.type = BlockType::Untyped;
}

MemoryBlockInfo* FindExistingMemoryInfo(void* p)
{
	uptr addr = reinterpret_cast<uptr>(p);
	Assert(addr > 0);

	uptr preprocessedAddr = addr >> BitsInPageAllocation;
	u32 blockIndex = preprocessedAddr & BlockInfoMask;
	u64 memoryBucketIndex = (preprocessedAddr >> BitsOfBlockInfoMask) & MemoryBucketMask;

	// TODO - Make this parallel arrays of some kind
	MemoryBlockInfoBucket& bucket = blockInfoHashTable[memoryBucketIndex];
	Assert(bucket.blockInfo);
	MemoryBlockInfo* blockInfo = nullptr;
	if (bucket.blockInfo)
	{
		blockInfo = &bucket.blockInfo[blockIndex];
		Assert(blockInfo->flag == OwnedBlockFlag::YesBlock);
		Assert(blockInfo->type != BlockType::Untyped);
	}

	return blockInfo;
}

//////////////////////////////////////////////////////////////////////////
// Fixed Block Size to Fixed Block Table Index
//////////////////////////////////////////////////////////////////////////
u8 FixedSizeToTableIndex(size_t alignedSize)
{
	// Bump up size to next 16 byte aligned size. Index 0 and 1 are associated with 16, so
	// this needs to ignore 0 index, which means bump up size to next alignment boundary
	size_t fixedSizeCacheIndex = (alignedSize + AllocationDefaultAlignment - 1) >> BitsForDefaultAlignment;
	Assert(fixedSizeCacheIndex > 0 && fixedSizeCacheIndex <= (MaxFixedTableSize >> BitsForDefaultAlignment));
	return fixedSizeToIndexCache[fixedSizeCacheIndex];
}
}