// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Graphics/RenderPassAttachments.hpp"

#include "Graphics/Vulkan/VulkanIndexBuffer.h"
#include "Graphics/Vulkan/VulkanVertexBuffer.h"

#include "BasicTypes/Color.hpp"

class VulkanFramebuffer;
class VulkanPipeline;
class VulkanCommandBuffer;
class VulkanDescriptorSet;
class WriteDescriptorSet;

struct RenderTargetDescription;
struct NativeRenderTargets;
struct GraphicsPipelineDescription;
struct VulkanBuffer;
struct VulkanTexture;
struct VulkanSampler;


// Describes what is currently processing within a render pass in Vulkan
// When a new render target is set, the current render pass ends and a new one begins

// TODO - Refactor the fuck outta this...
class VulkanRenderState
{
public:
	VulkanRenderState(VulkanDevice& device);

	// Changes the framebuffer being rendered to, while ending and beginning a new renderpass
	void BeginRenderPass(VulkanCommandBuffer& cmdBuffer, const BeginRenderPassInfo& beginInfo);
	void EndRenderPass(VulkanCommandBuffer& cmdBuffer);
	void SetGraphicsPipeline(const GraphicsPipelineDescription& pipelineDescription);
	void BindState(VulkanCommandBuffer& cmdBuffer);

	void SetUniformBuffer(const VulkanBuffer& buffer, u32 bindIndex);
	void SetStorageBuffer(const VulkanBuffer& buffer, u32 bindIndex);
	void SetTexture(const VulkanTexture& texture, const VulkanSampler& sampler, u32 bindIndex);

	bool IsTextureInRender(const VulkanTexture& texture);

	VulkanPipeline* GetCurrentPipeline() const { return currentPipeline; }
	bool InRenderPass() const { return inRenderPass; }

private:
	VulkanDevice& logicalDevice;
	WriteDescriptorSet* writeDescriptorSet = nullptr;
	VulkanFramebuffer* framebufferContext = nullptr;
	VulkanPipeline* currentPipeline = nullptr;
	bool inRenderPass = false;
};