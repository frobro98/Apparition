// Copyright 2020, Nathan Blane

#include "VulkanStorageBuffer.h"
#include "Utilities/MemoryUtilities.hpp"
#include "VulkanBufferAllocation.hpp"
#include "VulkanDevice.h"
#include "VulkanMemoryManager.hpp"

// VulkanStorageBuffer::~VulkanStorageBuffer()
// {
// 	if (mappedData != nullptr)
// 	{
// 		VulkanMemory::UnmapMemory(storageBuffer->allocatedMemory);
// 	}
// 
// 	VulkanMemory::DeallocateBuffer(storageBuffer);
// }

void VulkanStorageBuffer::Initialize(const VulkanDevice& device, u32 bufferSize)
{
	storageBuffer = device.GetMemoryManager().AllocateBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void VulkanStorageBuffer::UpdateStorage(const void* bufferData)
{
	storageBuffer->memory->LockForWrite();
	Memcpy(storageBuffer->memory->GetMappedPtr(), static_cast<size_t>(storageBuffer->memory->size), bufferData, static_cast<size_t>(storageBuffer->memory->size));
	storageBuffer->memory->Unlock();
}

