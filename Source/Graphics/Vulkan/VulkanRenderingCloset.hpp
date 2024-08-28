// Copyright 2020, Nathan Blane

#pragma once

// TODO - This isn't the greatest name for a class...should change at a later date

// The purpose of this file will be to hold all of the framebuffers and render passes that 
// Vulkan will be asked to produce. The framebuffers for the swapchain images will reside here,
// as well as the other render targets that might be needed. Render passes, being tied to 
// Framebuffers, will also reside here. 
//
// This may also be the place that stores the created pipeline objects as well. However, because
// those objects are so specific to only one part of the render api, this might not be the place.
// I will have to this about this a lot more...


#include "Containers/Map.h"
#include "Graphics/ResourceInitializationDescriptions.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"
#include "VulkanDefinitions.h"

class VulkanDevice;
class VulkanPipeline;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanPipelineLayout;

struct VulkanRenderingLayout
{
	VulkanRenderingLayout(const FixedArray<ColorDescription, MaxColorTargetCount>& colorDescs_, const DepthStencilDescription& depthDesc_)
	{
		numColorDescs = colorDescs_.Size();
		Memcpy(colorDescs, colorDescs_.GetData(), sizeof(ColorDescription) * numColorDescs);
		depthDesc = depthDesc_;
		hasDepthDesc = depthDesc.format != ImageFormat::Invalid;
	}

	ColorDescription colorDescs[MaxColorTargetCount] = {};
	DepthStencilDescription depthDesc = {};
	u32 numColorDescs = 0;
	bool hasDepthDesc = false;
};

inline bool operator==(const VulkanRenderingLayout& lhs, const VulkanRenderingLayout& rhs)
{
	return lhs.numColorDescs == rhs.numColorDescs && lhs.hasDepthDesc == rhs.hasDepthDesc &&
		Memcmp(lhs.colorDescs, rhs.colorDescs, sizeof(lhs.colorDescs)) == 0 && lhs.depthDesc == rhs.depthDesc;
}

inline u32 GetHash(const VulkanRenderingLayout& desc)
{
	struct HashableTargetDescription
	{
		FixedArray<ColorDescription, MaxColorTargetCount> colorDescs;
		DepthStencilDescription depthDesc;
	} hashDesc;
	hashDesc.colorDescs.Resize(desc.numColorDescs);
	Memcpy(hashDesc.colorDescs.GetData(), sizeof(hashDesc.colorDescs), desc.colorDescs, sizeof(desc.colorDescs));
	Memcpy(&hashDesc.depthDesc, sizeof(hashDesc.depthDesc), &desc.depthDesc, sizeof(desc.depthDesc));

	return fnv32(&hashDesc, sizeof(HashableTargetDescription));
}

class VulkanRenderingCloset
{
public:
	VulkanRenderingCloset(const VulkanDevice& device);
	~VulkanRenderingCloset();


	NODISCARD VulkanPipeline* FindOrCreatePipeline(const GraphicsPipelineDescription& desc);
	NODISCARD VulkanRenderPass* FindOrCreateRenderPass(const VulkanRenderingLayout& desc);
	// TODO - This isn't entirely correct, because a framebuffer could have the same attachment information, but not have the same render textures
	NODISCARD VulkanFramebuffer* FindOrCreateFramebuffer(const VulkanRenderingLayout& desc, const NativeRenderTargets& correspondingRTs);

	NODISCARD VkSampler FindOrCreateSampler(const SamplerDescription& params);
private:
	VulkanPipeline* CreatePipeline(const GraphicsPipelineDescription& desc);
	VulkanRenderPass* CreateRenderPass(const VulkanRenderingLayout& desc);
	VulkanFramebuffer* CreateFramebuffer(const VulkanRenderingLayout& desc, const NativeRenderTargets& correspondingRTs);
	VulkanPipelineLayout* ConfigurePipelineLayout(const GraphicsPipelineDescription& desc);
private:
	using SimilarFramebuffers = DynamicArray<VulkanFramebuffer*>;

	Map<GraphicsPipelineDescription, VulkanPipeline*> pipelineStore;
	Map<VulkanRenderingLayout, VulkanRenderPass*> renderPassStore;
	Map<VulkanRenderingLayout, SimilarFramebuffers> framebufferStore;
	Map<SamplerDescription, VkSampler> samplerStore;

	const VulkanDevice& logicalDevice;

};
