
#include "ImageFormatConversion.h"
#include "Containers/Map.h"

static Map<Apparition::ImageFormat::Type, VkFormat> ImageFormatToVkFormat;
static Map<VkFormat, Apparition::ImageFormat::Type> VkFormatToImageFormat;

void InitializeFormatMapping()
{
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::RGB_8norm, VK_FORMAT_R8G8B8_UNORM);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::RGB_8u, VK_FORMAT_R8G8B8_UINT);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::RGB_16f, VK_FORMAT_R16G16B16_SFLOAT);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::BGR_8u, VK_FORMAT_B8G8R8_UNORM);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::RGBA_8norm, VK_FORMAT_R8G8B8A8_UNORM);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::RGBA_8u, VK_FORMAT_R8G8B8A8_UINT);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::RGBA_16f, VK_FORMAT_R16G16B16A16_SFLOAT);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::BGRA_8norm, VK_FORMAT_B8G8R8A8_UNORM);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::Gray_8u, VK_FORMAT_R8_UNORM);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::BC1, VK_FORMAT_BC1_RGBA_UNORM_BLOCK);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::BC3, VK_FORMAT_BC3_UNORM_BLOCK);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::BC7, VK_FORMAT_BC7_UNORM_BLOCK);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::DS_32f_8u, VK_FORMAT_D32_SFLOAT_S8_UINT);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::DS_24f_8u, VK_FORMAT_D24_UNORM_S8_UINT);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::D_32f, VK_FORMAT_D32_SFLOAT);
    ImageFormatToVkFormat.Add(Apparition::ImageFormat::Invalid, VK_FORMAT_UNDEFINED);

    VkFormatToImageFormat.Add(VK_FORMAT_R8G8B8_UNORM, Apparition::ImageFormat::RGB_8norm);
    VkFormatToImageFormat.Add(VK_FORMAT_R8G8B8_UINT, Apparition::ImageFormat::RGB_8u);
    VkFormatToImageFormat.Add(VK_FORMAT_R16G16B16_SFLOAT, Apparition::ImageFormat::RGB_16f);
    VkFormatToImageFormat.Add(VK_FORMAT_B8G8R8_UNORM, Apparition::ImageFormat::BGR_8u);
    VkFormatToImageFormat.Add(VK_FORMAT_R8G8B8A8_UNORM, Apparition::ImageFormat::RGBA_8norm);
    VkFormatToImageFormat.Add(VK_FORMAT_R8G8B8A8_UINT, Apparition::ImageFormat::RGBA_8u);
    VkFormatToImageFormat.Add(VK_FORMAT_R16G16B16A16_SFLOAT, Apparition::ImageFormat::RGBA_16f);
    VkFormatToImageFormat.Add(VK_FORMAT_B8G8R8A8_UNORM, Apparition::ImageFormat::BGRA_8norm);
    VkFormatToImageFormat.Add(VK_FORMAT_R8_UNORM, Apparition::ImageFormat::Gray_8u);
    VkFormatToImageFormat.Add(VK_FORMAT_BC1_RGBA_UNORM_BLOCK, Apparition::ImageFormat::BC1);
    VkFormatToImageFormat.Add(VK_FORMAT_BC3_UNORM_BLOCK, Apparition::ImageFormat::BC3);
    VkFormatToImageFormat.Add(VK_FORMAT_BC7_UNORM_BLOCK, Apparition::ImageFormat::BC7);
    VkFormatToImageFormat.Add(VK_FORMAT_D32_SFLOAT_S8_UINT, Apparition::ImageFormat::DS_32f_8u);
    VkFormatToImageFormat.Add(VK_FORMAT_D24_UNORM_S8_UINT, Apparition::ImageFormat::DS_24f_8u);
    VkFormatToImageFormat.Add(VK_FORMAT_D32_SFLOAT, Apparition::ImageFormat::D_32f);
    VkFormatToImageFormat.Add(VK_FORMAT_UNDEFINED, Apparition::ImageFormat::Invalid);
}

Apparition::ImageFormat::Type VkFormatToApparitionFormat(VkFormat format)
{
    return VkFormatToImageFormat[format];
}

VkFormat ApparitionFormatToVkFormat(Apparition::ImageFormat::Type imageFormat)
{
    return ImageFormatToVkFormat[imageFormat];
}
