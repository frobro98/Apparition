// Copyright 2020, Nathan Blane

#include "Memory/MemoryCore.hpp"
#include "VulkanUniformBuffer.h"
#include "VulkanDevice.h"
#include "VulkanMemoryManager.hpp"
#include "VulkanBufferAllocation.hpp"

VulkanUniformBuffer::VulkanUniformBuffer(const VulkanDevice & device, u32 bufferSize)
	: logicalDevice(device)
{
	size = bufferSize;

	uniformBuffer = logicalDevice.GetMemoryManager().AllocateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
	logicalDevice.GetMemoryManager().DeallocateBuffer(*uniformBuffer);
}

void VulkanUniformBuffer::UpdateUniforms(const void* bufferData)
{
	uniformBuffer->memory->LockForWrite();
	Memcpy(uniformBuffer->memory->GetMappedPtr(), static_cast<size_t>(uniformBuffer->memory->size), bufferData, static_cast<size_t>(uniformBuffer->memory->size));
	uniformBuffer->memory->Unlock();
}

VulkanGlobalUniformBuffer::VulkanGlobalUniformBuffer(const VulkanDevice& device, u32 totalMemory)
	: logicalDevice(device)
{
	globalBuffer = logicalDevice.GetMemoryManager().AllocateBuffer(totalMemory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

VulkanGlobalUniformBuffer::~VulkanGlobalUniformBuffer()
{
	logicalDevice.GetMemoryManager().DeallocateBuffer(*globalBuffer);
}

void VulkanGlobalUniformBuffer::AddDataToBuffer(const void* bufferData, u32 dataSize)
{
	u32 alignedBufferPos = Align(bufferPosition, alignment);
	if (alignedBufferPos + dataSize >= globalBuffer->memory->blockSize)
	{
		alignedBufferPos = 0;
	}
	globalBuffer->memory->LockForWrite();
	Memcpy((u8*)globalBuffer->memory->GetMappedPtr() + alignedBufferPos, (size_t)dataSize, bufferData, (size_t)dataSize);
	globalBuffer->memory->Unlock();
}
