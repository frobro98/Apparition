// Copyright 2020, Nathan Blane

#include "VulkanBufferAllocation.hpp"
#include "VulkanDevice.h"

BufferGraphicsAllocation::BufferGraphicsAllocation(
	VulkanDevice& device,
	VkBuffer buff, 
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags memProperties,
	u32 size, 
	u32 alignment,
	u32 typeIndex, 
	bool canMap, bool cached, bool coherent)
	: GraphicsMemoryAllocation(
		device,
		memProperties, 
		size, 
		alignment,
		typeIndex, 
		canMap, cached, coherent),
	buffer(buff),
	usageFlags(usage)
{
	NOT_USED VkResult result = vkBindBufferMemory(device.GetNativeHandle(), buffer, graphicsMemory, 0);
	CHECK_VK(result);
}

BufferMemory* BufferGraphicsAllocation::CreateMemoryRange(VkDeviceSize bufferSize, u32 blockSize, u32 alignedOffset, u32 actualOffset)
{
	return new BufferMemory(*this, buffer, bufferSize, blockSize, alignedOffset, actualOffset);
}

BufferMemory::BufferMemory(BufferGraphicsAllocation& owningAlloc, VkBuffer buffer, VkDeviceSize size, u32 blockSize, u32 alignedOffset, u32 actualOffset)
	: GraphicsMemoryRange(owningAlloc, size, blockSize, alignedOffset, actualOffset),
	associatedBuffer(buffer)
{
}
