
#include "Device.h"
#include "Internal/DeviceManager.h"

namespace Apparition
{

DeviceHandle CreateDevice(const DeviceCreationParams& params)
{
	DeviceManager& deviceManager = DeviceManager::Get();
	return deviceManager.CreateDevice(params);
}

void DestroyDevice(DeviceHandle deviceHandle)
{
	DeviceManager& deviceManager = DeviceManager::Get();
	deviceManager.DestroyDevice(deviceHandle);
}

}
