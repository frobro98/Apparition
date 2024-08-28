// Copyright 2020, Nathan Blane

#pragma once

#include <mutex>

#include "Containers/DynamicArray.hpp"
#include "Graphics/ResourceInitializationDescriptions.hpp"
#include "VulkanDescriptorSetUtilities.hpp"

class VulkanDevice;
class VulkanRenderPass;
class VulkanDescriptorSet;
class VulkanCommandBuffer;
class VulkanDescriptorSetLayout;

struct GraphicsPipelineDescription;

class VulkanPipelineLayout
{
public:
	VulkanPipelineLayout(const VulkanDevice& device, DynamicArray<VulkanDescriptorSetLayout*>&& layouts);
	~VulkanPipelineLayout();

	inline DynamicArray<VulkanDescriptorSetLayout*> GetDescriptorSetLayouts() const { return descriptorSetLayouts; }
	inline VkPipelineLayout GetNativeHandle() const { return pipelineLayout; }
private:
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	DynamicArray<VulkanDescriptorSetLayout*> descriptorSetLayouts;
	const VulkanDevice* logicalDevice;
};

class VulkanPipeline
{
public:
	VulkanPipeline(const VulkanDevice& device);
	~VulkanPipeline();

	void Initialize(const VulkanPipelineLayout* layout, const GraphicsPipelineDescription& initInfo, VulkanRenderPass* renderPass);

	void Bind(VulkanCommandBuffer* cmdBuffer) const;
	void BindDescriptorSet(VulkanCommandBuffer* cmdBuffer, VulkanDescriptorSet* descriptorSet) const;
	VulkanDescriptorSet* GetUnusedDescriptorSet(VulkanCommandBuffer& cmdBuffer);

	inline WriteDescriptorSet& GetWriteDescriptorSet() { return writeDescriptorSet; }
	inline GraphicsPipelineDescription& GetDescription() { return desc; }
private:
	std::mutex descriptorMut;
	VkPipeline pipeline = VK_NULL_HANDLE;
	GraphicsPipelineDescription desc;
	WriteDescriptorSet writeDescriptorSet;
	const VulkanPipelineLayout* pipelineLayout;
	const VulkanDevice* logicalDevice;
	DynamicArray<VulkanDescriptorSet*> descriptorSets;
	u32 pad[1] = { 0 };
};
