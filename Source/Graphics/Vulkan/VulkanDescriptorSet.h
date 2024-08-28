// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanDefinitions.h"
#include "Containers/DynamicArray.hpp"
#include "Containers/Map.h"
#include "BasicTypes/Uncopyable.hpp"

#include "VulkanAbstractions.h"

class VulkanDevice;
class VulkanFence;
class VulkanCommandBuffer;
class WriteDescriptorSet;
struct TextureSampler;

class VulkanDescriptorSetLayout : private Uncopyable
{
public:
	VulkanDescriptorSetLayout(VulkanDevice& device);
	~VulkanDescriptorSetLayout();

	void AddDescriptorBinding(VkDescriptorType descriptorType, VkShaderStageFlags shaderStages, u32 binding);
	void BindLayout();

	inline VkDescriptorSetLayout GetNativeHandle() const { return descriptorSetLayout; }

	DynamicArray<VkDescriptorSetLayoutBinding> GetLayoutBindings() const;

private:
	void CombineRepeatBindings();

private:
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	Map<u32, VkDescriptorSetLayoutBinding> layoutBindings;
	VulkanDevice& logicalDevice;
};

class VulkanDescriptorSet : private Uncopyable
{
public:
	VulkanDescriptorSet(const VulkanDevice& device, VulkanFence* fence, VkDescriptorSet ds, VulkanDescriptorSetLayout* layout);

	inline VkDescriptorSet GetNativeHandle() const { return descriptorSet; }
	inline const VulkanFence* GetFence() const { return associatedFence; }
// 
// 	void SetUniformBufferInfo(const VulkanBuffer& uniformBuffer, uint32 bufferBinding);
// 	void SetStorageBufferInfo(const VulkanBuffer& storageBuffer, uint32 bufferBinding);
// 	void SetSampledTextureInfo(const VulkanTexture& texture, const TextureSampler& sampler, uint32 textureBinding);

	void UpdateDescriptorSet();
	void UpdateDescriptorSet(const WriteDescriptorSet& writeDescriptors);

public:
	VulkanCommandBuffer* associatedCommandBuffer;
	u64 associatedFenceSignal;

private:
	VkDescriptorSet descriptorSet;
	DynamicArray<VkDescriptorBufferInfo> bufferInfos;
	DynamicArray<VkDescriptorImageInfo> imageInfos;
	DynamicArray<VkWriteDescriptorSet> descriptorWrites;
	const VulkanDevice& logicalDevice;
	const VulkanDescriptorSetLayout* associatedLayout;
	const VulkanFence* associatedFence;
};
