
#include "Apparition/Image.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"

namespace Apparition
{
Image CreateImage(Device device, const ImageCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateImage(device, params);
}

void DestroyImage(Image image)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyImage(image);
}

ImageView CreateImageView(Image image, const ImageViewCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateImageView(image, params);
}

void DestroyImageView(ImageView imageView)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyImageView(imageView);
}
}