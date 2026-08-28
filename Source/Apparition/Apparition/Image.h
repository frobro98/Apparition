#pragma once

#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/Device.h"
#include "Apparition/ImageDescription.h"
#include "BasicTypes/Intrinsics.hpp"

struct VkImageView_T;
typedef struct VkImageView_T* VkImageView;


HANDLE_TYPE(AptnImage);
HANDLE_TYPE(AptnImageView);

// Only supports 2D images currently
struct AptnImageCreationParams
{
    u32 width = 0;
    u32 height = 0;
    AptnImageFormat format = AptnImageFormat::Invalid;
    u32 mipLevels = 0;
    AptnImageUsageFlags usageFlags = AptnImageUsageFlags::Max;

};

struct AptnImageViewCreationParams
{
    AptnImageFormat format = AptnImageFormat::Invalid;
    AptnImageAspectFlags aspect = AptnImageAspectFlags::Color;
    u32 mipCount = 1;
    u32 baseMipLevel = 0;
};

namespace Apparition
{
APPARITION_API AptnImage CreateImage(AptnDevice device, const AptnImageCreationParams& params);
APPARITION_API void DestroyImage(AptnImage image);

APPARITION_API AptnImageView CreateImageView(AptnImage image, const AptnImageViewCreationParams& params);
APPARITION_API void DestroyImageView(AptnImageView imageView);
}
