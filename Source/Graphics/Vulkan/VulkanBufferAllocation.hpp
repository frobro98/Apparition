// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanAllocation.hpp"

class BufferMemory;

struct BufferGraphicsAllocation : public GraphicsMemoryAllocation<BufferMemory>
{
public:
	BufferGraphicsAllocation(
		VulkanDevice& device,
		VkBuffer buffer,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memProperties,
		u32 size,
		u32 alignment,
		u32 typeIndex,
		bool canMap, bool cached, bool coherent
	);

	VkBuffer buffer;
	VkBufferUsageFlags usageFlags;

private:
	virtual BufferMemory* CreateMemoryRange(VkDeviceSize actualSize, u32 blockSize, u32 alignedOffset, u32 actualOffset) override;
	u32 pad[1] = { 0 };
};

class BufferMemory : public GraphicsMemoryRange<BufferGraphicsAllocation>
{
public:
	BufferMemory(
		BufferGraphicsAllocation& owningAlloc, 
		VkBuffer buffer,
		VkDeviceSize size,
		u32 blockSize,
		u32 alignedOffset,
		u32 actualOffset
	);

private:
	VkBuffer associatedBuffer;
};