#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Memory/MemoryCore.hpp"
#include "Memory/MemoryDefinitions.hpp"

namespace Memory::Internal
{
constexpr u64 OSAllocationBitMask = 0xffffffffffff0000;
constexpr u64 BitsInPageAllocation = 16;

constexpr u32 MemoryBucketCount = 128; // This is the upper bits of a ptr address that isn't being used for the pools
constexpr u64 MemoryBucketMask = MemoryBucketCount - 1;
constexpr u16 FixedSizeCacheCount = 2048;
constexpr u16 BlockInfosPerBucket = 2048;
constexpr u64 BlockInfoMask = BlockInfosPerBucket - 1;
constexpr u32 BitsOfMemoryBucketMask = 7;
constexpr u32 BitsOfBlockInfoMask = 11;

constexpr size_t TotalSmallFixedTableSizes = 64;
// Used for some sizes so that the header can fit into the page
constexpr size_t FixedHeaderAdjust = 16;
constexpr size_t MaxFixedTableSize = (PageAllocationSize / 2) - FixedHeaderAdjust; // Adjust to allow for FreedBlock header

static forceinline bool IsAllocationFromOS(void* ptr)
{
	return IsAligned(ptr, OSAllocationAlignment);
}
}
