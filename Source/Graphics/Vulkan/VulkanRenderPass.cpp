// Copyright 2020, Nathan Blane

#include "Utilities/Array.hpp"
#include "VulkanRenderPass.h"
#include "VulkanDevice.h"
#include "VulkanCreateInfos.h"
#include "VulkanUtilities.h"
#include "VulkanRenderingCloset.hpp"
#include "Graphics/ResourceInitializationDescriptions.hpp"

VulkanRenderPass::VulkanRenderPass(const VulkanDevice& device, const VulkanRenderingLayout& targets)
// TODO - There isn't necessarily a depth attachment. Assuming there is, but needs to change later...
	: attachments(targets.hasDepthDesc ? targets.numColorDescs + 1 : targets.numColorDescs),
	logicalDevice(&device)
{

	DynamicArray<VkAttachmentReference> attachmentRefs(targets.numColorDescs);
	for (u32 i = 0; i < attachmentRefs.Size(); ++i)
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		const ColorDescription& colorDescription = targets.colorDescs[i];
		colorAttachment.format = MusaFormatToVkFormat(colorDescription.format);
		colorAttachment.samples = static_cast<VkSampleCountFlagBits>(colorDescription.sampleCount);
		colorAttachment.loadOp = MusaLoadToVk(GetLoadOperation(colorDescription.op));
		colorAttachment.storeOp = MusaStoreToVk(GetStoreOperation(colorDescription.op));

		attachments[i] = colorAttachment;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = i;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachmentRefs[i] = colorAttachmentRef;
	}

	VkAttachmentReference depthAttachmentRef = {};
	if(targets.hasDepthDesc)
	{
		const DepthStencilDescription& depthDescription = targets.depthDesc;
		VkAttachmentDescription depthAttachment = {};
		VkImageLayout depthLayout = depthDescription.access == RenderTargetAccess::Read ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.initialLayout = depthLayout;
		depthAttachment.finalLayout = depthLayout;

		depthAttachment.format = MusaFormatToVkFormat(depthDescription.format);
		depthAttachment.samples = static_cast<VkSampleCountFlagBits>(depthDescription.sampleCount);
		ColorTargetOperations depthOp = GetDepthOperation(depthDescription.op);
		ColorTargetOperations stencilOp = GetStencilOperation(depthDescription.op);
		depthAttachment.loadOp = MusaLoadToVk(GetLoadOperation(depthOp));
		depthAttachment.storeOp = MusaStoreToVk(GetStoreOperation(depthOp));
		depthAttachment.stencilLoadOp = MusaLoadToVk(GetLoadOperation(stencilOp));
		depthAttachment.stencilStoreOp = MusaStoreToVk(GetStoreOperation(stencilOp));

		Assert(logicalDevice->IsFormatSupported(depthAttachment.format, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT));
		attachments[attachments.Size() - 1] = depthAttachment;

		depthAttachmentRef.attachment = attachments.Size() - 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = attachmentRefs.Size();
	subpass.pColorAttachments = attachmentRefs.GetData();
	subpass.pDepthStencilAttachment = targets.hasDepthDesc ? &depthAttachmentRef : nullptr;

	// TODO - Find out what a subpass dependency means for the pipeline
	VkSubpassDependency dependencies[2];

// 	VkPipelineStageFlags dstStage0 = attachmentRefs.Size() > 0 ?
// 		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT :
// 		VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
// 	VkPipelineStageFlags srcStage1 = attachmentRefs.Size() > 0 ?
// 		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT :
// 		VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
// 	VkPipelineStageFlags dstStage1 = attachmentRefs.Size() > 0 ?
// 		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT :
// 		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
// 
// 	VkAccessFlags accessFlags = attachmentRefs.Size() > 0 ?
// 		(u32)(VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT) :
// 		(u32)(VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
//
// 	dependencies[0] = {};
// 	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
// 	dependencies[0].dstSubpass = 0;
// 	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
// 	dependencies[0].dstStageMask = dstStage0;
// 	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
// 	dependencies[0].dstAccessMask = accessFlags;
// 	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
// 
// 	dependencies[1] = {};
// 	dependencies[1].srcSubpass = 0;
// 	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
// 	dependencies[1].srcStageMask = srcStage1;
// 	dependencies[1].dstStageMask = dstStage1;
// 	dependencies[1].srcAccessMask = accessFlags;
// 	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
// 	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassInfo = Vk::RenderPassInfo(attachments.GetData(), attachments.Size(), &subpass, 1, dependencies, 0/*(u32)ArraySize(dependencies)*/);
	NOT_USED VkResult result = vkCreateRenderPass(logicalDevice->GetNativeHandle(), &renderPassInfo, nullptr, &renderPass);
	CHECK_VK(result);
}

VulkanRenderPass::~VulkanRenderPass()
{
	Assert(renderPass != VK_NULL_HANDLE);
	vkDestroyRenderPass(logicalDevice->GetNativeHandle(), renderPass, nullptr);
}
