// Copyright 2020, Nathan Blane

#include "Memory.hpp"
#include "Internal/MemoryAllocationInternal.hpp"

namespace Memory
{
namespace Internal
{
bool isInitialized = false;
}

void InitializeMemory()
{
	Assert(!Internal::isInitialized);
	NOT_USED PlatformMemory::PlatformMemoryInfo memInfo = PlatformMemory::GetPlatformMemoryInfo();

	// Use memory info to generate however many allocation meta data buckets there are, forever

	Memory::Internal::InitializeFixedBlockTable();

	Memory::Internal::InitializeMemoryInfoTable();

	Internal::isInitialized = true;
}

void DeinitializeMemory()
{
	Assert(Internal::isInitialized);

	// TODO - Replace these with a possible log output
	Debug::Printf("Big memory allocations: {}\n", Memory::Internal::memoryStats.allocatedBigMemory);
	Debug::Printf("Fixed memory allocations: {}\n", Memory::Internal::memoryStats.allocatedFixedMemory);

	Internal::isInitialized = false;
}
}
