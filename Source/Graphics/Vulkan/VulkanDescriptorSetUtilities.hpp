// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanAbstractions.h"
#include "VulkanTexture.h"
#include "VulkanSampler.hpp"
#include "Containers/DynamicArray.hpp"

class VulkanDescriptorSetLayout;

struct WriteBufferDescriptor final
{
	VkDescriptorType descType;
	u32 bindingIndex;
	VkDescriptorBufferInfo bufferInfo;
};

struct WriteImageDescriptor final
{
	VkDescriptorType descType;
	u32 bindingIndex;
	VkDescriptorImageInfo imageInfo;
};

struct WriteDescriptor final
{
	WriteBufferDescriptor* bufferDescriptor = nullptr;
	WriteImageDescriptor* imageDescriptor = nullptr;
};

class WriteDescriptorSet final
{
public:
	void SetupWriteDescriptors(const VulkanDescriptorSetLayout& layout);

	void SetBuffer(VkDescriptorType type, const VulkanBuffer& buffer, u32 bindIndex);
	void SetTexture(VkDescriptorType type, const VulkanTexture& texture, const VulkanSampler& sampler, u32 bindIndex);

	const DynamicArray<WriteDescriptor>& GetWriteDescriptors() const { return writeDescriptors; }

private:
	DynamicArray<WriteBufferDescriptor> bufferDescriptors;
	DynamicArray<WriteImageDescriptor> imageDescriptors;
	// Arranged by the binding index
	DynamicArray<WriteDescriptor> writeDescriptors;
	u32 numBuffers = 0;
	u32 numImages = 0;
};
