
#include "Device.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"

namespace Apparition
{
Device CreateDevice(const DeviceCreationParams& params)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.CreateDevice(params);
}

void DestroyDevice(Device deviceHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.DestroyDevice(deviceHandle);
}

u32 GetGraphicsQueueIndex(Device deviceHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	DeviceInternal& deviceInternals = deviceManager.DeviceInternalFrom(deviceHandle);
	return deviceInternals.graphicsFamilyIndex;

	return 0;
}

u32 GetComputeQueueIndex(Device deviceHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	DeviceInternal& deviceInternals = deviceManager.DeviceInternalFrom(deviceHandle);
	return deviceInternals.computeFamilyIndex;
}

u32 GetTransferQueueIndex(Device deviceHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	DeviceInternal& deviceInternals = deviceManager.DeviceInternalFrom(deviceHandle);
	return deviceInternals.transferFamilyIndex;
}

VkDevice GetVulkanDevice(Device deviceHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	DeviceInternal& deviceInternals = deviceManager.DeviceInternalFrom(deviceHandle);
	return deviceInternals.device;
}

}
