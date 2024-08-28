// Copyright 2020, Nathan Blane

#include "VulkanRenderState.hpp"
#include "Debugging/Assertion.hpp"
#include "VulkanCommandBuffer.h"
#include "VulkanPipeline.h"
#include "VulkanRenderingCloset.hpp"
#include "VulkanDevice.h"
#include "VulkanDescriptorSet.h"
#include "GraphicsInterface.hpp"
#include "VulkanAbstractions.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanBufferAllocation.hpp"
#include "VulkanFramebuffer.h"

VulkanRenderState::VulkanRenderState(VulkanDevice& device)
	: logicalDevice(device)
{
}

void VulkanRenderState::BeginRenderPass(VulkanCommandBuffer& cmdBuffer, const BeginRenderPassInfo& beginInfo)
{
	Assert(!InRenderPass());
	
	// Convert clear colors and clear DS to VkClearValues
	DynamicArray<VkClearValue> vkClearColors;
	u32 numColorAttachments = beginInfo.colorAttachments.Size();
	vkClearColors.Resize(beginInfo.targets.depthTarget ? numColorAttachments + 1 : numColorAttachments);
	for (u32 i = 0; i < numColorAttachments; ++i)
	{
		vkClearColors[i].color = {
			beginInfo.clearColors[i].r,
			beginInfo.clearColors[i].g,
			beginInfo.clearColors[i].b,
			beginInfo.clearColors[i].a
		};
	}

	if (beginInfo.targets.depthTarget != nullptr)
	{
		vkClearColors[vkClearColors.Size() - 1].depthStencil = { beginInfo.clearDS.clearDepth, beginInfo.clearDS.clearStencil };
	}

	// Get Framebuffer
	const VulkanRenderingLayout renderLayout(beginInfo.colorAttachments, beginInfo.depthAttachment);
	const NativeRenderTargets nativeTextures = [&beginInfo]
	{
		NativeRenderTargets textures;
		textures.colorTargets = beginInfo.targets.colorTargets;
		textures.depthTarget = beginInfo.targets.depthTarget;
		textures.extents = beginInfo.targets.extents;
		return textures;
	}();

	VulkanFramebuffer* newTargetFramebuffer = logicalDevice.GetRenderingStorage()->FindOrCreateFramebuffer(renderLayout, nativeTextures);

	cmdBuffer.BeginRenderpass(newTargetFramebuffer, vkClearColors);

	framebufferContext = newTargetFramebuffer;
	inRenderPass = true;
}

void VulkanRenderState::EndRenderPass(VulkanCommandBuffer& cmdBuffer)
{
	Assert(InRenderPass());
	Assert(cmdBuffer.IsInRenderPass());

	cmdBuffer.EndRenderPass();

	framebufferContext = nullptr;
	inRenderPass = false;
}

void VulkanRenderState::SetGraphicsPipeline(const GraphicsPipelineDescription& pipelineDescription)
{
// 	if (cmdBuffer.GetLevel() == VK_COMMAND_BUFFER_LEVEL_PRIMARY)
// 	{
// 		Assert(cmdBuffer.IsInRenderPass());
// 	}

	VulkanPipeline* newPipeline = logicalDevice.GetRenderingStorage()->FindOrCreatePipeline(pipelineDescription);

	currentPipeline = newPipeline;
	writeDescriptorSet = &newPipeline->GetWriteDescriptorSet();
}

void VulkanRenderState::BindState(VulkanCommandBuffer& cmdBuffer)
{
	VulkanDescriptorSet* ds = currentPipeline->GetUnusedDescriptorSet(cmdBuffer);
	ds->UpdateDescriptorSet(*writeDescriptorSet);

 	currentPipeline->Bind(&cmdBuffer);
 	currentPipeline->BindDescriptorSet(&cmdBuffer, ds);
}

void VulkanRenderState::SetUniformBuffer(const VulkanBuffer& buffer, u32 bindIndex)
{
	Assert(writeDescriptorSet);
	writeDescriptorSet->SetBuffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, buffer, bindIndex);
}

void VulkanRenderState::SetStorageBuffer(const VulkanBuffer& buffer, u32 bindIndex)
{
	Assert(writeDescriptorSet);
	writeDescriptorSet->SetBuffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, buffer, bindIndex);
}

void VulkanRenderState::SetTexture(const VulkanTexture& texture, const VulkanSampler& sampler, u32 bindIndex)
{
	Assert(writeDescriptorSet);
	writeDescriptorSet->SetTexture(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, texture, sampler, bindIndex);
}

bool VulkanRenderState::IsTextureInRender(const VulkanTexture& texture)
{
	return framebufferContext && framebufferContext->ContainsRT(texture);
}

