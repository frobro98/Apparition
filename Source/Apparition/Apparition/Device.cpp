
#include "Device.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"

namespace Apparition
{
DeviceHandle CreateDevice(const DeviceCreationParams& params)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.CreateDevice(params);
}

void DestroyDevice(DeviceHandle deviceHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.DestroyDevice(deviceHandle);
}

u32 GetGraphicsQueueIndex(DeviceHandle deviceHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	DeviceInternal& deviceInternals = deviceManager.GetDeviceInternals(deviceHandle);
	return deviceInternals.graphicsFamilyIndex;

	return 0;
}

u32 GetComputeQueueIndex(DeviceHandle deviceHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	DeviceInternal& deviceInternals = deviceManager.GetDeviceInternals(deviceHandle);
	return deviceInternals.computeFamilyIndex;
}

u32 GetTransferQueueIndex(DeviceHandle deviceHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	DeviceInternal& deviceInternals = deviceManager.GetDeviceInternals(deviceHandle);
	return deviceInternals.transferFamilyIndex;
}

VkDevice GetVulkanDevice(DeviceHandle deviceHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	DeviceInternal& deviceInternals = deviceManager.GetDeviceInternals(deviceHandle);
	return deviceInternals.device;
}

}
