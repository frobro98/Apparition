
#include "Apparition/Image.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"

namespace Apparition
{
AptnImage CreateImage(AptnDevice device, const AptnImageCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateImage(device, params);
}

void DestroyImage(AptnImage image)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyImage(image);
}

AptnImageView CreateImageView(AptnImage image, const AptnImageViewCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateImageView(image, params);
}

void DestroyImageView(AptnImageView imageView)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyImageView(imageView);
}
}