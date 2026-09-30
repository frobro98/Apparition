
#include "Device.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"

namespace Apparition
{
// Currently, AptnDevice is not supported by the macro that defines "IsValid" for other
// handle types. It might be good to get this going later down the road.
bool IsValid(AptnDevice handle)
{
    Assert(apparition.deviceManager);
    if (handle == AptnInvalidHandle)
    {
        return false;
    }

    const u32 deviceHandleIndex = GetHandleIndex(handle);
    // Purposely not calling the handle overload because it does its own "IsValid" call, which
    // will infinitely recurse
    const DeviceInternal& deviceInternal = apparition.deviceManager->DeviceInternalFrom(deviceHandleIndex);
    return deviceInternal.handle != VK_NULL_HANDLE;
}

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
