// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanDefinitions.h"

class VulkanDevice;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;

class VulkanDescriptorPool
{
public:
	VulkanDescriptorPool(VulkanDevice& device);
	~VulkanDescriptorPool();

	VulkanDescriptorSet* AllocateDescriptorSet(VulkanDescriptorSetLayout* setLayout);


private:
	VkDescriptorPool descriptorPool;
	VulkanDevice& logicalDevice;

	u32 maxSamplerPoolSize = 0;
	u32 maxUniformBufferPoolSize = 0;
	u32 maxStorageBufferPoolSize = 0;
	u32 maxDynamicStorageBufferPoolSize = 0;
	u32 maxDynamicUniformBufferPoolSize = 0;
	u32 maxSampledImagePoolSize = 0;
	u32 maxStorageImagePoolSize = 0;
	u32 maxInputAttachmentPoolSize = 0;

	u32 currentSamplerPool = 0;
	u32 currentUniformBufferPool = 0;
	u32 currentStorageBufferPool = 0;
	u32 currentDynamicStorageBufferPool = 0;
	u32 currentDynamicUniformBufferPool = 0;
	u32 currentSampledImagePool = 0;
	u32 currentStorageImagePool = 0;
	u32 currentInputAttachmentPool = 0;
};