// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanAllocation.hpp"

class ImageMemory;

struct ImageGraphicsAllocation : public GraphicsMemoryAllocation<ImageMemory>
{
	ImageGraphicsAllocation(
		VulkanDevice& device,
		VkImage image,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags memProperties,
		u32 size,
		u32 alignment,
		u32 typeIndex,
		bool canMap, bool cached, bool coherent
	);

	inline VkImageUsageFlags GetImageUsage() const { return usageFlags; }
	inline VkImage GetImageHandle() const { return image; }

	VkImage image;
	VkImageUsageFlags usageFlags;

protected:
	virtual ImageMemory* CreateMemoryRange(VkDeviceSize actualSize, u32 blockSize, u32 alignedOffset, u32 actualOffset) override;
	u32 pad[1] = { 0 };
};

class ImageMemory : public GraphicsMemoryRange<ImageGraphicsAllocation>
{
public:
	ImageMemory(
		ImageGraphicsAllocation& owningAlloc, 
		VkImage image,
		VkDeviceSize size,
		u32 blockSize, 
		u32 alignedOffset, 
		u32 actualOffset
	);

private:
	VkImage associatedImage;
};