#pragma once

#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/Device.h"
#include "Apparition/ImageDescription.h"
#include "BasicTypes/Intrinsics.hpp"

namespace Apparition
{

// Only supports 2D images currently
struct ImageCreationParams
{
    u32 width = 0;
    u32 height = 0;
    ImageFormat::Type format = ImageFormat::Invalid;
    u32 mipLevels = 0;
    ImageUsageFlags usageFlags = 0;

};

struct Image
{
    u64 handle;
};

APPARITION_API Image CreateImage(Device device);
APPARITION_API void DestroyImage(Image image);

//APPARITION_API RetVal GetImageDescription(Image image);

struct ImageViewCreationParams
{
    Image image = { InvalidHandle };
    ImageFormat::Type format = ImageFormat::Invalid;
    u32 mipCount = 1;
    u32 baseMipLevel = 0;
};

struct ImageView
{
    u64 handle;
};

APPARITION_API ImageView CreateImageView(Image image);
APPARITION_API void DestroyImageView(ImageView imageView);
}
