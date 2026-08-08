
#include "Buffer.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/HandleDefinitions.h"
#include "Internal/VulkanDefinitions.h"

namespace Apparition
{
AptnBuffer CreateBuffer(AptnDevice device, const AptnBufferCreationParams& params)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.CreateBuffer(device, params);
}

void DestroyBuffer(AptnBuffer buffer)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.DestroyBuffer(buffer);
}

void* MapBuffer(AptnBuffer buffer)
{
	Assert(apparition.deviceManager);
	DeviceInternal& deviceInternal = GetDeviceInternal(buffer);
	u32 handleIndex = GetHandleIndex(buffer);
	BufferInternal& bufferInternal = GetBufferInternalFromIndex(deviceInternal, handleIndex);
	Assert(bufferInternal.isMappable);

	void* mappedData = nullptr;
	VkResult result = vmaMapMemory(deviceInternal.allocator, bufferInternal.allocation, &mappedData);
	CHECK_VK(result);

	return mappedData;
}

void UnmapBuffer(AptnBuffer buffer)
{
	Assert(apparition.deviceManager);
	DeviceInternal& deviceInternal = GetDeviceInternal(buffer);
	u32 handleIndex = GetHandleIndex(buffer);
	BufferInternal& bufferInternal = GetBufferInternalFromIndex(deviceInternal, handleIndex);

	vmaUnmapMemory(deviceInternal.allocator, bufferInternal.allocation);
}
}
