#pragma once

#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/Device.h"
#include "BasicTypes/Intrinsics.hpp"

namespace Apparition
{

struct ImageCreationParams
{

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

};

struct ImageView
{
    u64 handle;
};

APPARITION_API ImageView CreateImageView(Image image);
APPARITION_API void DestroyImageView(ImageView imageView);
}
