// Copyright 2020, Nathan Blane

#include "VulkanIndexBuffer.h"
#include "VulkanUtilities.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanMemoryManager.hpp"
#include "VulkanBufferAllocation.hpp"

// TODO - The magic number "3" must be defined somewhere...
VulkanIndexBuffer::VulkanIndexBuffer(VulkanDevice& device, u64 sizeInBytes, u32 indexSize)
	: logicalDevice(device),
	indexStride(indexSize)
{
	size = sizeInBytes;

	indexBuffer = logicalDevice.GetMemoryManager().AllocateBuffer(
		sizeInBytes,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
	logicalDevice.GetMemoryManager().DeallocateBuffer(*indexBuffer);
}
