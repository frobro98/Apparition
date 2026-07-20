
#include "Apparition/Image.h"

namespace Apparition
{
Image CreateImage(Device device, const ImageCreationParams& params)
{
    Assert(false);
    UNUSED(device, params);
    return { InvalidHandle };
}

void DestroyImage(Image image)
{
    Assert(false);
    UNUSED(image);
}

ImageView CreateImageView(Image image, const ImageViewCreationParams& params)
{
    Assert(false);
    UNUSED(image, params);
    return { InvalidHandle };
}

void DestroyImageView(ImageView imageView)
{
    Assert(false);
    UNUSED(imageView);
}

VkImageView GetVulkanHandle(ImageView viewHandle)
{
    Assert(false);
    UNUSED(viewHandle);
    return nullptr;
}
}