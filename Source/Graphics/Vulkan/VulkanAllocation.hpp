// Copyright 2020, Nathan Blane

#pragma once

#include <mutex>

#include "Memory/MemoryCore.hpp"
#include "Math/MathFunctions.hpp"
#include "VulkanDefinitions.h"
#include "Containers/DynamicArray.hpp"
#include "BasicTypes/Ranges.hpp"
#include "Graphics/Vulkan/VulkanDevice.h"
#include "Graphics/ResourceLockPtr.hpp"


class GraphicsAllocation
{
public:
	virtual ~GraphicsAllocation() = default;
};

template <class Suballoc>
class GraphicsMemoryAllocation : public GraphicsAllocation
{
public:
	GraphicsMemoryAllocation(
		VulkanDevice& device,
		VkMemoryPropertyFlags memProperties,
		VkDeviceSize size,
		u32 alignment,
		u32 typeIndex,
		bool canMap, bool cached, bool coherent
	);

	virtual ~GraphicsMemoryAllocation();

	bool TrySelectMemoryRange(VkDeviceSize requestedSize, u32 alignment, Suballoc*& range);

	void LockForWrite(VkDeviceSize size, u32 offset);
	void Unlock();

// 	void* MapData(VkDeviceSize size, uint32 offset);
// 	void UnmapData();

	inline u32 GetTotalMemoryRanges() const { return freeAllocationBlocks.Size(); }
	inline u32 GetAlignment() const { return offsetAlignment; }

	inline bool IsCoherent() const { return isCoherent; }
	inline bool IsCached() const { return isCached; }
	inline bool CanBeMapped() const { return canMapData; }
	inline bool IsMapped() const { return mappedData != nullptr; }
	void* GetMappedPtr() const
	{
		Assert(IsMapped());
		return mappedData;
	}

	inline VkDeviceMemory GetMemoryHandle() const { return graphicsMemory; }
	inline VkMemoryPropertyFlags GetMemoryProperties() const { return memoryProperties; }

protected:
	virtual Suballoc* CreateMemoryRange(VkDeviceSize actualSize, u32 blockSize, u32 alignedOffset, u32 actualOffset) = 0;

protected:
	DynamicArray<URange> freeAllocationBlocks;
	DynamicArray<Suballoc*> suballocationBlocks; 
	VkDeviceMemory graphicsMemory;
	VulkanDevice& logicalDevice;

private:
	std::mutex allocationMutex;
	void* mappedData = nullptr;
	VkMemoryPropertyFlags memoryProperties;
	VkDeviceSize allocationSize;
	u32 usedSpaceSize;
	u32 offsetAlignment;
	// This is for accessing information about the heap this allocation is apart
	u32 memoryTypeIndex;
	u32 canMapData : 1;
	u32 isCoherent : 1;
	u32 isCached : 1;
};

template <class Alloc>
struct GraphicsMemoryRange
{
	GraphicsMemoryRange(
		Alloc& owningAlloc,
		VkDeviceSize allocSize,
		u32 blockSize,
		u32 alignedOffset,
		u32 actualOffset
	);

	virtual ~GraphicsMemoryRange();

	void LockForWrite();
	void Unlock();

// 	void MapData();
// 	void UnmapData();

	inline bool CanBeMapped() const { return owningAllocation.CanBeMapped(); }
	inline bool IsMapped() const { return owningAllocation.IsMapped(); }
	void* GetMappedPtr() const
	{
		Assert(CanBeMapped() && IsMapped());
		return owningAllocation.GetMappedPtr();
	}

	Alloc& owningAllocation;
	const VkDeviceSize size;

	// This is the size of the entire block of memory requested by the suballocation
	// Could be the same as total size
	const u32 blockSize;
	// Aligned offset for when Vk is interacting with the memory
	const u32 alignedOffset;
	// Actual offset of memory within the actual allocation
	const u32 actualOffset;
};

//////////////////////////////////////////////////////////////////////////
// GraphicsMemoryAllocation
//////////////////////////////////////////////////////////////////////////

template <class Suballoc>
GraphicsMemoryAllocation<Suballoc>::GraphicsMemoryAllocation(
	VulkanDevice& device,
	VkMemoryPropertyFlags memProperties,
	VkDeviceSize size,
	u32 memAlignment,
	u32 typeIndex,
	bool canMap, bool cached, bool coherent)
	: logicalDevice(device),
	memoryProperties(memProperties),
	allocationSize(size),
	usedSpaceSize(0),
	offsetAlignment(memAlignment),
	memoryTypeIndex(typeIndex),
	canMapData(canMap),
	isCoherent(coherent),
	isCached(cached)
{
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = size;
	allocInfo.memoryTypeIndex = typeIndex;
	// TODO - URange is an interesting concept, but I need something more flexible to cover size_t, uint64, uint16, etc...
	URange range = { 0, (u32)size };
	freeAllocationBlocks.Add(range);

	NOT_USED VkResult result = vkAllocateMemory(device.GetNativeHandle(), &allocInfo, nullptr, &graphicsMemory);
	CHECK_VK(result);
}

template<class Suballoc>
inline GraphicsMemoryAllocation<Suballoc>::~GraphicsMemoryAllocation()
{
	for (auto subAlloc : suballocationBlocks)
	{
		delete subAlloc;
	}
	if (IsMapped())
	{
		Unlock();
	}
	vkFreeMemory(logicalDevice.GetNativeHandle(), graphicsMemory, nullptr);
}

template<class Suballoc>
inline bool GraphicsMemoryAllocation<Suballoc>::TrySelectMemoryRange(VkDeviceSize requestedSize, u32 alignment, Suballoc *& memoryRange)
{
	u32 adjustedAlignment = Math::Max(alignment, offsetAlignment);
	for (u32 i = 0; i < freeAllocationBlocks.Size(); ++i)
	{
		// TODO - Need to change this shit to be able to use size_t/VkDeviceSize by default instead of using uint32
		URange& allocRange = freeAllocationBlocks[i];
		u32 actualOffset = allocRange.start;
		u32 alignedOffset = Align(actualOffset, adjustedAlignment);
		u32 offsetAdjustedDiff = alignedOffset - actualOffset;
		u32 totalRequestedMemory = offsetAdjustedDiff + (u32)requestedSize;
		if (totalRequestedMemory <= allocRange.size)
		{
			if (totalRequestedMemory < allocRange.size)
			{
				allocRange.start += totalRequestedMemory;
				allocRange.size -= totalRequestedMemory;
			}
			else // equal
			{
				freeAllocationBlocks.Remove(i);
			}

			memoryRange = CreateMemoryRange(requestedSize, totalRequestedMemory, alignedOffset, actualOffset);

			return true;
		}
	}

	return false;
}

template<class Suballoc>
inline void GraphicsMemoryAllocation<Suballoc>::LockForWrite(VkDeviceSize size, u32 offset)
{
	Assert(CanBeMapped());
	Assert(!IsMapped());
	allocationMutex.lock();
	NOT_USED VkResult result = vkMapMemory(logicalDevice.GetNativeHandle(), graphicsMemory, offset, size, 0, &mappedData);
	CHECK_VK(result);
}

template<class Suballoc>
inline void GraphicsMemoryAllocation<Suballoc>::Unlock()
{
	Assert(CanBeMapped());
	Assert(IsMapped());
	vkUnmapMemory(logicalDevice.GetNativeHandle(), graphicsMemory);
	mappedData = nullptr;
	allocationMutex.unlock();
}

// template <class Suballoc>
// void* GraphicsMemoryAllocation<Suballoc>::MapData(VkDeviceSize size, uint32 offset)
// {
// 	Assert(CanBeMapped());
// 	Assert(!IsMapped());
// 	CHECK_VK(vkMapMemory(logicalDevice.GetNativeHandle(), graphicsMemory, offset, size, 0, &mappedData));
// 	return mappedData;
// }
// 
// template <class Suballoc>
// void GraphicsMemoryAllocation<Suballoc>::UnmapData()
// {
// 	Assert(CanBeMapped());
// 	Assert(IsMapped());
// 	vkUnmapMemory(logicalDevice.GetNativeHandle(), graphicsMemory);
// 	mappedData = nullptr;
// }

//////////////////////////////////////////////////////////////////////////
// GraphicsMemoryRange
//////////////////////////////////////////////////////////////////////////

template <class Alloc>
GraphicsMemoryRange<Alloc>::GraphicsMemoryRange(
	Alloc& owningAlloc,
	VkDeviceSize allocSize,
	u32 blckSize,
	u32 alignedOffset,
	u32 actualOffset
)
	: owningAllocation(owningAlloc),
	size(allocSize),
	blockSize(blckSize),
	alignedOffset(alignedOffset),
	actualOffset(actualOffset)
{
}

template<class Alloc>
inline GraphicsMemoryRange<Alloc>::~GraphicsMemoryRange()
{
}

template<class Alloc>
inline void GraphicsMemoryRange<Alloc>::LockForWrite()
{
	owningAllocation.LockForWrite(size, alignedOffset);
}

template<class Alloc>
inline void GraphicsMemoryRange<Alloc>::Unlock()
{
	owningAllocation.Unlock();
}
