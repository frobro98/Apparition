
#include "Buffer.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/HandleDefinitions.h"
#include "Internal/VulkanDefinitions.h"

namespace Apparition
{
Buffer CreateBuffer(Device device, const BufferCreationParams& params)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.CreateBuffer(device, params);
}

void DestroyBuffer(Buffer buffer)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.DestroyBuffer(buffer);
}

void* MapBuffer(Buffer buffer)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	const u32 deviceIndex = GetDeviceIndexFromHandle(buffer);
	DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
	u32 handleIndex = GetHandleIndex(buffer);
	BufferResourceInternal& bufferInternal = deviceInternal.bufferResources[handleIndex - 1];
	Assert(bufferInternal.isMappable);

	void* mappedData = nullptr;
	VkResult result = vmaMapMemory(deviceInternal.allocator, bufferInternal.allocation, &mappedData);
	CHECK_VK(result);

	return mappedData;
}

void UnmapBuffer(Buffer buffer)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	const u32 deviceIndex = GetDeviceIndexFromHandle(buffer);
	DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
	u32 handleIndex = GetHandleIndex(buffer);
	BufferResourceInternal& bufferInternal = deviceInternal.bufferResources[handleIndex - 1];

	vmaUnmapMemory(deviceInternal.allocator, bufferInternal.allocation);
}
}
