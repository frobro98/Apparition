
#include "Backbuffer.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/ImageFormatConversion.h"

namespace Apparition
{
void SetupBackbuffer(Device device, const BackbufferSetupParams& params)
{
    Assert(apparition.deviceManager);

    return apparition.deviceManager->SetupBackbuffer(device, params);
}

void TeardownBackbuffer(Device device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.TeardownBackbuffer(device);
}
ImageFormat::Type GetBackbufferFormat(Device device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    const DeviceInternal& deviceInternals = deviceManager.GetDeviceInternals(device);
    return VkFormatToApparitionFormat(deviceInternals.backbuffer.format);
}
u32 GetBackbufferVkFormat(Device device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    const DeviceInternal& deviceInternals = deviceManager.GetDeviceInternals(device);
    return deviceInternals.backbuffer.format;
}
}