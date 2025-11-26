
#include "Backbuffer.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/ImageFormatConversion.h"

namespace Apparition
{
void SetupBackbuffer(DeviceHandle device, const BackbufferSetupParams& params)
{
    Assert(apparition.deviceManager);

    return apparition.deviceManager->SetupBackbuffer(device, params);
}

void TeardownBackbuffer(DeviceHandle device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.TeardownBackbuffer(device);
}
ImageFormat::Type GetBackbufferFormat(DeviceHandle device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    const DeviceInternal& deviceInternals = deviceManager.GetDeviceInternals(device);
    return VkFormatToApparitionFormat(deviceInternals.backbuffer.format);
}
u32 GetBackbufferVkFormat(DeviceHandle device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    const DeviceInternal& deviceInternals = deviceManager.GetDeviceInternals(device);
    return deviceInternals.backbuffer.format;
}
}