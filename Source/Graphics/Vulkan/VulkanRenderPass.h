// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanDefinitions.h"

#include "Containers/DynamicArray.hpp"
#include "Graphics/RenderPassAttachments.hpp"

class VulkanDevice;
struct VulkanRenderingLayout;

class VulkanRenderPass
{
public:
	VulkanRenderPass(const VulkanDevice& device, const VulkanRenderingLayout& targets);
	~VulkanRenderPass();

	inline VkRenderPass GetNativeHandle() const { return renderPass; }
	inline DynamicArray<VkAttachmentDescription> GetAttachments() const { return attachments; }

private:
	VkRenderPass renderPass;
	DynamicArray<VkAttachmentDescription> attachments;
	const VulkanDevice* logicalDevice;
};
