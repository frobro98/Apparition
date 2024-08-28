// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Utilities/MemoryUtilities.hpp"
#include "VulkanDevice.h"

struct VulkanBuffer;

constexpr size_t AllocationSizeBytes = MegabytesAsBytes(1);

struct TempAlloc
{
	void* allocData;
	VkBuffer buffer;
	VkDeviceSize allocSize;
	u32 offset;
};

struct VulkanFrameTempAllocation
{
	VulkanFrameTempAllocation(const VulkanDevice& device);
	~VulkanFrameTempAllocation();

	TempAlloc AllocateTempMemory(VkDeviceSize allocSize, u32 allocAlignment);
	void ClearSuballocations();

	void* freeAllocPtr;
	void* mappedBeginningPtr;
	VkDeviceSize allocationSize;

private:
	const VulkanDevice& logicalDevice;
	VulkanBuffer* allocBuffer;
};