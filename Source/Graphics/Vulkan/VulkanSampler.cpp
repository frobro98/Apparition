// Copyright 2020, Nathan Blane

#include "VulkanSampler.hpp"
#include "VulkanRenderingCloset.hpp"
#include "VulkanDevice.h"

VulkanSampler::VulkanSampler(const VulkanDevice& device, const SamplerDescription& params)
	: logicalDevice(device)
{
	sampler = logicalDevice.GetRenderingStorage()->FindOrCreateSampler(params);
}

VulkanSampler::~VulkanSampler()
{
	vkDestroySampler(logicalDevice.GetNativeHandle(), sampler, nullptr);
}
