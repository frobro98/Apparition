
#include "ImageFormatConversion.h"
#include "Containers/Map.h"
#include "Utilities/Array.hpp"

static VkFormat vkFormats[] = {
    VK_FORMAT_R8G8B8_UNORM,         // RGB_8norm
    VK_FORMAT_R8G8B8_UINT,          // RGB_8u
    VK_FORMAT_R16G16B16_SFLOAT,     // RGB_16f
    VK_FORMAT_B8G8R8_UNORM,         // BGR_8norm
    VK_FORMAT_R8G8B8A8_UNORM,       // RGBA_8norm
    VK_FORMAT_R8G8B8A8_UINT,        // RGBA_8u
    VK_FORMAT_R16G16B16A16_SFLOAT,  // RGBA_16f
    VK_FORMAT_B8G8R8A8_UNORM,       // BGRA_8norm
    VK_FORMAT_R8_UNORM,             // Gray_8norm
    VK_FORMAT_BC1_RGBA_UNORM_BLOCK, // BC1
    VK_FORMAT_BC3_UNORM_BLOCK,      // BC3
    VK_FORMAT_BC7_UNORM_BLOCK,      // BC7
    VK_FORMAT_D32_SFLOAT_S8_UINT,   // DS_32f_8u
    VK_FORMAT_D24_UNORM_S8_UINT,    // DS_24f_8u
    VK_FORMAT_D32_SFLOAT,           // D_32f
    VK_FORMAT_UNDEFINED             // Invalid
};
static_assert(ArraySize(vkFormats) == AptnImageFormat::Count);

static Map<VkFormat, AptnImageFormat::Type> VkFormatToImageFormat;

void InitializeFormatMapping()
{
    VkFormatToImageFormat.Add(VK_FORMAT_R8G8B8_UNORM, AptnImageFormat::RGB_8norm);
    VkFormatToImageFormat.Add(VK_FORMAT_R8G8B8_UINT, AptnImageFormat::RGB_8u);
    VkFormatToImageFormat.Add(VK_FORMAT_R16G16B16_SFLOAT, AptnImageFormat::RGB_16f);
    VkFormatToImageFormat.Add(VK_FORMAT_B8G8R8_UNORM, AptnImageFormat::BGR_8norm);
    VkFormatToImageFormat.Add(VK_FORMAT_R8G8B8A8_UNORM, AptnImageFormat::RGBA_8norm);
    VkFormatToImageFormat.Add(VK_FORMAT_R8G8B8A8_UINT, AptnImageFormat::RGBA_8u);
    VkFormatToImageFormat.Add(VK_FORMAT_R16G16B16A16_SFLOAT, AptnImageFormat::RGBA_16f);
    VkFormatToImageFormat.Add(VK_FORMAT_B8G8R8A8_UNORM, AptnImageFormat::BGRA_8norm);
    VkFormatToImageFormat.Add(VK_FORMAT_R8_UNORM, AptnImageFormat::Gray_8norm);
    VkFormatToImageFormat.Add(VK_FORMAT_BC1_RGBA_UNORM_BLOCK, AptnImageFormat::BC1);
    VkFormatToImageFormat.Add(VK_FORMAT_BC3_UNORM_BLOCK, AptnImageFormat::BC3);
    VkFormatToImageFormat.Add(VK_FORMAT_BC7_UNORM_BLOCK, AptnImageFormat::BC7);
    VkFormatToImageFormat.Add(VK_FORMAT_D32_SFLOAT_S8_UINT, AptnImageFormat::DS_32f_8u);
    VkFormatToImageFormat.Add(VK_FORMAT_D24_UNORM_S8_UINT, AptnImageFormat::DS_24f_8u);
    VkFormatToImageFormat.Add(VK_FORMAT_D32_SFLOAT, AptnImageFormat::D_32f);
    VkFormatToImageFormat.Add(VK_FORMAT_UNDEFINED, AptnImageFormat::Invalid);
}

AptnImageFormat::Type VkFormatToApparition(VkFormat format)
{
    return VkFormatToImageFormat[format];
}

VkFormat ApparitionFormatToVk(AptnImageFormat::Type imageFormat)
{
    return vkFormats[imageFormat];
}
