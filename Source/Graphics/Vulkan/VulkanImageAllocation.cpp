// Copyright 2020, Nathan Blane

#include "VulkanImageAllocation.hpp"
#include "VulkanDevice.h"

ImageGraphicsAllocation::ImageGraphicsAllocation(
	VulkanDevice& device, 
	VkImage img,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags memProperties,
	u32 size, 
	u32 alignment,
	u32 typeIndex, 
	bool canMap, bool cached, bool coherent
)
	: GraphicsMemoryAllocation(device,memProperties,size,alignment,typeIndex,canMap,cached,coherent),
	image(img),
	usageFlags(usage)
{
	NOT_USED VkResult result = vkBindImageMemory(device.GetNativeHandle(), image, graphicsMemory, 0);
	CHECK_VK(result);
}

ImageMemory* ImageGraphicsAllocation::CreateMemoryRange(VkDeviceSize actualSize, u32 blockSize, u32 alignedOffset, u32 actualOffset)
{
	return new ImageMemory(*this, image, actualSize, blockSize, alignedOffset, actualOffset);
}

ImageMemory::ImageMemory(ImageGraphicsAllocation& owningAlloc, VkImage img, VkDeviceSize imgSize, u32 blockSize, u32 alignedOffset, u32 actualOffset)
	: GraphicsMemoryRange(owningAlloc, imgSize, blockSize, alignedOffset, actualOffset),
	associatedImage(img)
{
}
