// Copyright 2020, Nathan Blane

#include "Memory/MemoryCore.hpp"
#include "VulkanFrameTempAllocation.hpp"
#include "VulkanMemoryManager.hpp"
#include "VulkanAbstractions.h"
#include "VulkanBufferAllocation.hpp"

VulkanFrameTempAllocation::VulkanFrameTempAllocation(const VulkanDevice& device)
	: logicalDevice(device)
{
	// Create Buffer allocation with vertex and index buffer usage of size AllocationSizeBytes
	allocBuffer = logicalDevice.GetMemoryManager().AllocateBuffer(AllocationSizeBytes,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	// TODO - Unlike other instances of the memory object, this one owns the entire allocation. It doesn't make sense to incur a mutex lock for an owning allocation...
	allocBuffer->memory->LockForWrite();
	mappedBeginningPtr = allocBuffer->memory->GetMappedPtr();
	freeAllocPtr = mappedBeginningPtr;
	allocationSize = allocBuffer->memory->size;

}

VulkanFrameTempAllocation::~VulkanFrameTempAllocation()
{
	// Deallocate the buffer allocation
	logicalDevice.GetMemoryManager().DeallocateBuffer(*allocBuffer);
}

TempAlloc VulkanFrameTempAllocation::AllocateTempMemory(VkDeviceSize allocSize, u32 allocAlignment)
{
	// Assert if the remaining buffer size can fit this new allocation
	// This is a big deal if it can't, because there isn't a way to handle this sort of issue
	u8* dataPtr = Align((u8*)freeAllocPtr, allocAlignment);
	Assert(dataPtr + allocSize <= (u8*)mappedBeginningPtr + allocationSize);

	// Fill the memory structure up with the data required
	// TODO - Have a helper function do the pointer difference calc
	u32 offset = (u32)((u8*)freeAllocPtr - (u8*)mappedBeginningPtr);
	freeAllocPtr = dataPtr + allocSize;

	return TempAlloc
	{
		dataPtr,
		allocBuffer->handle,
		allocSize,
		offset
	};
}

void VulkanFrameTempAllocation::ClearSuballocations()
{
	// Move the current allocation pointer to the beginning of the mapped memory
	freeAllocPtr = mappedBeginningPtr;
}
