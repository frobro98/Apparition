// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Debugging/Assertion.hpp"
#include "Memory/Internal/MemoryInternalDefinitions.hpp"

namespace Memory::Internal
{
struct FixedBlockPool;

enum class BlockType : u32
{
	Untyped,
	FixedSmallBlock,
	DynamicBlock
};

enum class OwnedBlockFlag : u32
{
	NoBlock = 0xdead,
	YesBlock = 0xdada
};

// Needs to be 16 byte aligned, so sizeof(MemoryBlockInfo) must equal 32 bytes
struct MemoryBlockInfo
{
	size_t allocatedSize = 0; // Size of associated block, alignment taken into consideration
	OwnedBlockFlag flag = OwnedBlockFlag::NoBlock;
	BlockType type = BlockType::Untyped; // Useful to know what block of memory I'm dealing with
	FixedBlockPool* associatedPool;
	u64 pad[1] = {};
};
static_assert(sizeof(MemoryBlockInfo) == 32);

// TODO - Investigate whether my theory about total memory allocations is true. 
// I think it is, if we weren't in the virtual memory space...
// If this thought is correct, then there will be collisions for buckets
struct MemoryBlockInfoBucket
{
	MemoryBlockInfo* blockInfo = nullptr;
};

}
