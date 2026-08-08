
#include "Device.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"

namespace Apparition
{
AptnDevice CreateDevice(const AptnDeviceCreationParams& params)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.CreateDevice(params);
}

void DestroyDevice(AptnDevice deviceHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.DestroyDevice(deviceHandle);
}
}
