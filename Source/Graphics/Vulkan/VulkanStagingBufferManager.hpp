// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanDefinitions.h"

#include "Containers/DynamicArray.hpp"
#include "VulkanAbstractions.h"

class VulkanFence;
class VulkanDevice;
class VulkanCommandBuffer;

// NOTE: Keeping the staging buffers away from the memory manager seems to make more sense than 
// Combining them, for some reason. Will think about this more later when I actually have time...

struct VulkanStagingBuffer
{
	VulkanStagingBuffer(VkBuffer handle, VkDeviceSize size);

	void ReleaseMemory(const VulkanDevice& device);

	VkBuffer stagingHandle;
	VkDeviceSize stagingSize;
	VkDeviceMemory backingAllocation;
	void* mappedData = nullptr;
	u32 pad[1] = { 0 };
};

struct DeferredStageReleaseElement
{
	VulkanStagingBuffer* stagingBuffer;
	VulkanFence* associatedFence;
};

class VulkanStagingBufferManager
{
public:
	VulkanStagingBufferManager(const VulkanDevice& device);
	~VulkanStagingBufferManager();

	VulkanStagingBuffer* AllocateStagingBuffer(VkDeviceSize bufferSize);
	void ReleaseStagingBuffer(VulkanCommandBuffer& cmdBuffer, VulkanStagingBuffer& buffer);

	void ProcessDeferredReleases();

private:
	VulkanStagingBuffer* CreateFreshBuffer(VkDeviceSize size);
	void AllocateMemoryFor(VulkanStagingBuffer& buffer);
	

private:
	DynamicArray<VulkanStagingBuffer*> resonentStagingBuffers;
	DynamicArray<DeferredStageReleaseElement> deferredReleaseBuffers;
	VulkanBuffer* biggestBuffer = nullptr;
	const VulkanDevice& logicalDevice;
};
