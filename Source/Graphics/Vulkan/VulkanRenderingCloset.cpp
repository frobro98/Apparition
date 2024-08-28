// Copyright 2020, Nathan Blane

#include "VulkanRenderingCloset.hpp"
#include "VulkanShaders.h"
#include "VulkanDescriptorSet.h"
#include "VulkanPipeline.h"
#include "VulkanUtilities.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanCreateInfos.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanDescriptorLayoutManager.h"
#include "VulkanShaderHeader.hpp"

VulkanRenderingCloset::VulkanRenderingCloset(const VulkanDevice& device)
	: logicalDevice(device)
{
}

VulkanRenderingCloset::~VulkanRenderingCloset()
{
	for (auto pipelinePair : pipelineStore)
	{
		delete pipelinePair.second;
	}
	for (auto renderPassPair : renderPassStore)
	{
		delete renderPassPair.second;
	}
	for (auto similarFBPair : framebufferStore)
	{
		for (auto framebuffer : similarFBPair.second)
		{
			delete framebuffer;
		}
	}
	for (auto samplerPair : samplerStore)
	{
		vkDestroySampler(logicalDevice.GetNativeHandle(), samplerPair.second, nullptr);
	}
}

VulkanPipeline* VulkanRenderingCloset::FindOrCreatePipeline(const GraphicsPipelineDescription& desc)
{
	VulkanPipeline* pipeline = nullptr;
	if (!pipelineStore.TryFind(desc, pipeline))
	{
		pipeline = CreatePipeline(desc);
	}
	return pipeline;
}

VulkanRenderPass* VulkanRenderingCloset::FindOrCreateRenderPass(const VulkanRenderingLayout& desc)
{
	VulkanRenderPass* renderPass = nullptr;
	if (!renderPassStore.TryFind(desc, renderPass))
	{
		renderPass = CreateRenderPass(desc);
	}
	return renderPass;
}

VulkanFramebuffer* VulkanRenderingCloset::FindOrCreateFramebuffer(const VulkanRenderingLayout& desc, const NativeRenderTargets& correspondingRTs)
{
	SimilarFramebuffers* frameBuffers = framebufferStore.Find(desc);
	VulkanFramebuffer* framebuffer = nullptr;
	if (frameBuffers)
	{
		for (u32 i = 0; i < frameBuffers->Size(); ++i)
		{
			VulkanFramebuffer* fb = (*frameBuffers)[i];
			if (fb->ContainsRTs(correspondingRTs))
			{
				framebuffer = fb;
				break;
			}
		}
		if (!framebuffer)
		{
			framebuffer = CreateFramebuffer(desc, correspondingRTs);
			frameBuffers->Add(framebuffer);
		}
	}
	else
	{
		framebuffer = CreateFramebuffer(desc, correspondingRTs);
		SimilarFramebuffers similarFBs(1);
		similarFBs[0] = framebuffer;
		framebufferStore.Add(desc, MOVE(similarFBs));
	}
	return framebuffer;
}

VkSampler VulkanRenderingCloset::FindOrCreateSampler(const SamplerDescription& params)
{
	VkSampler* foundSampler = samplerStore.Find(params);
	if (!foundSampler)
	{
		VkSamplerCreateInfo samplerInfo = Vk::SamplerInfo(params);
		VkSampler sampler;
		vkCreateSampler(logicalDevice.GetNativeHandle(), &samplerInfo, nullptr, &sampler);
		samplerStore.Add(params, sampler);
		return sampler;
	}

	return *foundSampler;
}

VulkanPipeline* VulkanRenderingCloset::CreatePipeline(const GraphicsPipelineDescription& desc)
{
	// Create render layout structure
	VulkanRenderingLayout renderLayout(desc.colorAttachments, desc.depthAttachment);

	VulkanRenderPass* renderPass = FindOrCreateRenderPass(renderLayout);
	VulkanPipelineLayout* layout = ConfigurePipelineLayout(desc);
	
	VulkanPipeline* pipeline = new VulkanPipeline(logicalDevice);
	pipeline->Initialize(layout, desc, renderPass);
	pipelineStore.Add(desc, pipeline);

	return pipeline;
}

VulkanRenderPass* VulkanRenderingCloset::CreateRenderPass(const VulkanRenderingLayout& desc)
{
	VulkanRenderPass* renderPass = new VulkanRenderPass(logicalDevice, desc);
	renderPassStore.Add(desc, renderPass);
	return renderPass;
}

VulkanFramebuffer* VulkanRenderingCloset::CreateFramebuffer(const VulkanRenderingLayout& desc, const NativeRenderTargets& correspondingRTs)
{
	VulkanRenderPass* renderPass = FindOrCreateRenderPass(desc);
	VulkanFramebuffer* framebuffer = new VulkanFramebuffer(logicalDevice);
	framebuffer->Initialize(desc, correspondingRTs, renderPass);
	return framebuffer;
}

VulkanPipelineLayout* VulkanRenderingCloset::ConfigurePipelineLayout(const GraphicsPipelineDescription& desc)
{
	VulkanDescriptorSetLayout* dsLayout = GetDescriptorLayoutManager().CreateSetLayout();

	{
		const VulkanVertexShader* vertShader = reinterpret_cast<const VulkanVertexShader*>(desc.vertexShader);
		const DynamicArray<SpirvBuffer>& buffers = vertShader->shaderData.shaderHeader.buffers;
		const DynamicArray<SpirvSampler>& samplers = vertShader->shaderData.shaderHeader.samplers;

		for (const auto& buffer : buffers)
		{
			dsLayout->AddDescriptorBinding(buffer.bufferType, VK_SHADER_STAGE_VERTEX_BIT, buffer.bindIndex);
		}

		for (const auto& sampler : samplers)
		{
			dsLayout->AddDescriptorBinding(sampler.samplerType, VK_SHADER_STAGE_VERTEX_BIT, sampler.bindIndex);
		}
	}

	if (desc.fragmentShader)
	{
		const VulkanFragmentShader* fragShader = reinterpret_cast<const VulkanFragmentShader*>(desc.fragmentShader);
		const DynamicArray<SpirvBuffer>& buffers = fragShader->shaderData.shaderHeader.buffers;
		const DynamicArray<SpirvSampler>& samplers = fragShader->shaderData.shaderHeader.samplers;

		for (const auto& buffer : buffers)
		{
			dsLayout->AddDescriptorBinding(buffer.bufferType, VK_SHADER_STAGE_FRAGMENT_BIT, buffer.bindIndex);
		}

		for (const auto& sampler : samplers)
		{
			dsLayout->AddDescriptorBinding(sampler.samplerType, VK_SHADER_STAGE_FRAGMENT_BIT, sampler.bindIndex);
		}
	}

	dsLayout->BindLayout();

	return new VulkanPipelineLayout(logicalDevice, { dsLayout });
}
