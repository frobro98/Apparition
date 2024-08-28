// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"
#include "VulkanDefinitions.h"

class VulkanDevice;
class VulkanCommandBuffer;
class VulkanDescriptorSet;
class VulkanPipelineLayout;
class VulkanDescriptorSetLayout;
struct VulkanComputeShader;

class VulkanComputePipeline
{
public:
	VulkanComputePipeline(VulkanDevice& device);
	~VulkanComputePipeline();

	void Initialize(const VulkanPipelineLayout* layout, VulkanComputeShader* shader);

	void Bind(VulkanCommandBuffer* cmdBuffer) const;
	void BindDescriptorSet(VulkanCommandBuffer* cmdBuffer, VulkanDescriptorSet* descriptorSet) const;
	VulkanDescriptorSet* GetUnusedDescriptorSet();
	void ResetPipeline();
private:
	VkPipeline pipeline;
	const VulkanPipelineLayout* pipelineLayout = nullptr;
	VulkanDevice& logicalDevice;
	DynamicArray<VulkanDescriptorSet*> descriptorSets;
	u32 unusedSetIndex;
};
