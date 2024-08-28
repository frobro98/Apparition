// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanDefinitions.h"
#include "VulkanBufferAllocation.hpp"
#include "Containers/DynamicArray.hpp"
#include "BasicTypes/Uncopyable.hpp"
#include "BasicTypes/Unmoveable.hpp"

class VulkanDevice;
class GraphicsAllocation;
struct BufferGraphicsAllocation;
struct ImageGraphicsAllocation;
struct VulkanBuffer;
struct VulkanImage;

class VulkanMemoryManager : private Uncopyable, private Unmoveable
{
public:
	VulkanMemoryManager(VulkanDevice& device);
	~VulkanMemoryManager();

	VulkanImage* AllocateImage(
		u32 imgWidth, u32 imgHeight, VkFormat imgFormat,
		u32 mipMapLevels, VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags memoryProperties
	);
	void DeallocateImage(VulkanImage& image);

	VulkanBuffer* AllocateBuffer(
		VkDeviceSize bufferSize, VkBufferUsageFlags usageFlags,
		VkMemoryPropertyFlags memoryFlags
	);
	void DeallocateBuffer(VulkanBuffer& buffer);

	void FreePendingAllocations();

private:
	template <class Allocation>
	bool TryCreateBufferFrom(const DynamicArray<Allocation*>& allocList, VkDeviceSize bufferSize, u32 alignment, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags, VulkanBuffer*& subAlloc);

	VulkanBuffer* CreateNewAllocation(VkDeviceSize bufferSize, u32 alignment, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags);
	u32 QueryMemoryType(u32 typeFilter, VkMemoryPropertyFlags propertyFlags);

	BufferGraphicsAllocation& AllocateBufferBlock(VkDeviceSize bufferSize, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags);
private:
	static constexpr size_t DefaultAllocationSize = MegabytesAsBytes(1);

	DynamicArray<GraphicsAllocation*> allocations;
	DynamicArray<GraphicsAllocation*> allocationsToFree;
	// These don't need to be raw pointers. Try and find a way to represent this better
	DynamicArray<BufferGraphicsAllocation*> freeBufferAllocations;
	DynamicArray<BufferGraphicsAllocation*> usedBufferAllocations;
	DynamicArray<ImageGraphicsAllocation*> imageAllocations;

	VulkanDevice& logicalDevice;

	u32 peakAllocationCount = 0;
	u32 largestAllocationSize = 0;
};

//////////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////////

template<class Allocation>
inline bool VulkanMemoryManager::TryCreateBufferFrom(const DynamicArray<Allocation*>& allocList, VkDeviceSize bufferSize, u32 alignment, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags, VulkanBuffer*& subAlloc)
{
	for (auto allocation : allocList)
	{
		VkBufferUsageFlags usage = allocation->usageFlags;
		VkMemoryPropertyFlags properties = allocation->GetMemoryProperties();
		if ((usage & usageFlags) == usageFlags &&
			(properties & memoryFlags) == memoryFlags)
		{
			BufferMemory* bufferMemory;
			if (allocation->TrySelectMemoryRange((u32)bufferSize, alignment, bufferMemory))
			{
				subAlloc = new VulkanBuffer(logicalDevice, allocation->buffer, *bufferMemory);
				return true;
			}
		}
	}

	return false;
}
