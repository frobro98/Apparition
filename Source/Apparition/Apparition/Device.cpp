
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

}
