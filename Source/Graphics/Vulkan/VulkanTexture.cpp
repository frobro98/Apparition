// Copyright 2020, Nathan Blane

#include "VulkanTexture.h"
#include "VulkanDevice.h"

VulkanTexture::VulkanTexture(VulkanDevice& device, VulkanImage& allocedImage)
	: image(&allocedImage),
	logicalDevice(device)
{
	imageView = image->CreateView();
}

VulkanTexture::~VulkanTexture()
{
	vkDestroyImageView(logicalDevice.GetNativeHandle(), imageView, nullptr);
}