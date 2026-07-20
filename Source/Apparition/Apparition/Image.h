#pragma once

#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/Device.h"
#include "Apparition/ImageDescription.h"
#include "BasicTypes/Intrinsics.hpp"

struct VkImageView_T;
typedef struct VkImageView_T* VkImageView;

namespace Apparition
{

HANDLE_TYPE(Image);
HANDLE_TYPE(ImageView);

// Only supports 2D images currently
struct ImageCreationParams
{
    u32 width = 0;
    u32 height = 0;
    ImageFormat::Type format = ImageFormat::Invalid;
    u32 mipLevels = 0;
    ImageUsageFlags usageFlags = 0;

};


APPARITION_API Image CreateImage(Device device, const ImageCreationParams& params);
APPARITION_API void DestroyImage(Image image);

struct ImageViewCreationParams
{
    Image image;
    ImageFormat::Type format = ImageFormat::Invalid;
    ImageAspect::Type aspect = ImageAspect::Color;
    u32 mipCount = 1;
    u32 baseMipLevel = 0;
};

APPARITION_API ImageView CreateImageView(Image image, const ImageViewCreationParams& params);
APPARITION_API void DestroyImageView(ImageView imageView);

// TEMPORARILY HERE
APPARITION_API VkImageView GetVulkanHandle(ImageView viewHandle);
}
