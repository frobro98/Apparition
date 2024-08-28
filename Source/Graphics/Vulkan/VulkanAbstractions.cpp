// Copyright 2020, Nathan Blane

#include "VulkanAbstractions.h"
#include "VulkanCreateInfos.h"
#include "VulkanDevice.h"

VulkanImage::~VulkanImage()
{
	vkDestroyImage(device->GetNativeHandle(), handle, nullptr);
}

VkImageView VulkanImage::CreateView() const
{
	VkImageViewCreateInfo imageViewInfo = Vk::ImageViewInfo(handle, mipmapLevels, format, aspectFlags);
	imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;

	VkImageView imageView = VK_NULL_HANDLE;
	NOT_USED VkResult result = vkCreateImageView(device->GetNativeHandle(), &imageViewInfo, nullptr, &imageView);
	CHECK_VK(result);
	return imageView;
}

VulkanBuffer::~VulkanBuffer()
{
	vkDestroyBuffer(device.GetNativeHandle(), handle, nullptr);
}
