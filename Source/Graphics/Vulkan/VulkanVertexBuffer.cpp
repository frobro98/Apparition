// Copyright 2020, Nathan Blane

#include "VulkanVertexBuffer.h"
#include "VulkanUtilities.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanMemoryManager.hpp"
#include "VulkanBufferAllocation.hpp"

VulkanVertexBuffer::VulkanVertexBuffer(VulkanDevice& device, u64 sizeInBytes)
	: logicalDevice(device)
{
	// TODO - Need to figure out where this goes
	size = sizeInBytes;

	// TODO - Get staging buffers working. Need to cache them until they're used
	vertexBuffer = logicalDevice.GetMemoryManager().AllocateBuffer(
		sizeInBytes,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
	logicalDevice.GetMemoryManager().DeallocateBuffer(*vertexBuffer);
}

// VulkanWeightsBuffer::VulkanWeightsBuffer(VulkanDevice& device, const DynamicArray<VertexBoneWeights>& weights)
// 	: logicalDevice(device)
// {
// 	if (weights.Size() > 0)
// 	{
// 		uint32 vertexBufferSize = sizeof(VertexBoneWeights) * weights.Size();
// 
// 		// TODO - Get staging buffers working. Need to cache them until they're used
// 		weightsBuffer = logicalDevice.GetMemoryManager().AllocateBuffer(
// 			vertexBufferSize,
// 			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
// 			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
// 		);
// 
// 		{
// 			weightsBuffer->memory->LockForWrite();
// 			Memcpy(weightsBuffer->memory->GetMappedPtr(), vertexBufferSize, weights.GetData(), vertexBufferSize);
// 			weightsBuffer->memory->Unlock();
// 		}
// 
// 	}
// 	else
// 	{
// 		weightsBuffer = nullptr;
// 	}
// 
// 	// 	VulkanBuffer stagingBuffer = VulkanMemory::AllocateBuffer(
// 	// 		vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
// 	// 		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
// 	// 	);
// 
// 	// 	void* data = VulkanMemory::MapMemory(stagingBuffer.allocatedMemory, stagingBuffer.size);
// 	// 	Memcpy(data, vertexBufferSize, vertices.GetData(), vertexBufferSize);
// 	// 	VulkanMemory::UnmapMemory(stagingBuffer.allocatedMemory);
// 
// 	// 	vertexBuffer = VulkanMemory::AllocateBuffer(
// 	// 		vertexBufferSize,
// 	// 		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
// 	// 		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
// 	// 	);
// 
// 	//CopyToDeviceBuffer(stagingBuffer, vertexBuffer);
// 
// 	//VulkanMemory::DeallocateBuffer(stagingBuffer);
// }
// 
// VulkanWeightsBuffer::~VulkanWeightsBuffer()
// {
// 	logicalDevice.GetMemoryManager().DeallocateBuffer(*weightsBuffer);
// }
// 
// void VulkanWeightsBuffer::Bind(VulkanCommandBuffer* cmdBuffer) const
// {
// 	Assert(cmdBuffer->IsInRenderPass());
// 	VkDeviceSize offsets[] = { 0 };
// 	vkCmdBindVertexBuffers(cmdBuffer->GetNativeHandle(), 1, 1, &weightsBuffer->handle, offsets);
// }
