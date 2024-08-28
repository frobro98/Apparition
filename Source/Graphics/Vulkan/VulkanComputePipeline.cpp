// Copyright 2020, Nathan Blane

#include "VulkanComputePipeline.h"
#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorPool.h"
#include "VulkanPipeline.h"
#include "VulkanShaders.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"

VulkanComputePipeline::VulkanComputePipeline(VulkanDevice& device)
	: logicalDevice(device)
{
}

VulkanComputePipeline::~VulkanComputePipeline()
{
}

void VulkanComputePipeline::Initialize(const VulkanPipelineLayout* layout, VulkanComputeShader* shader)
{
	pipelineLayout = layout;

	VkPipelineShaderStageCreateInfo stage = {};
	stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stage.module = shader->shaderData.module;
	stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	stage.pName = "main";

	VkComputePipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.layout = layout->GetNativeHandle();
	pipelineCreateInfo.stage = stage;

	NOT_USED VkResult result = vkCreateComputePipelines(logicalDevice.GetNativeHandle(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
	CHECK_VK(result);
}

void VulkanComputePipeline::Bind(VulkanCommandBuffer* cmdBuffer) const
{
	vkCmdBindPipeline(cmdBuffer->GetNativeHandle(), VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
}

void VulkanComputePipeline::BindDescriptorSet(VulkanCommandBuffer* cmdBuffer, VulkanDescriptorSet* descriptorSet) const
{
	VkDescriptorSet ds = descriptorSet->GetNativeHandle();
	vkCmdBindDescriptorSets(cmdBuffer->GetNativeHandle(),
		VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout->GetNativeHandle(),
		0, 1, &ds, 0, nullptr);
}

VulkanDescriptorSet * VulkanComputePipeline::GetUnusedDescriptorSet()
{
	static u32 numDescriptorsCreated = 0;
	VulkanDescriptorSet* descriptorSet = nullptr;
	if (unusedSetIndex < descriptorSets.Size())
	{
		descriptorSet = descriptorSets[unusedSetIndex];
		++unusedSetIndex;
	}
	else
	{
		++numDescriptorsCreated;
		VulkanDescriptorPool* pool = logicalDevice.GetActivePool();
		descriptorSet = pool->AllocateDescriptorSet(pipelineLayout->GetDescriptorSetLayouts()[0]);

		Assert(descriptorSet != nullptr);
		descriptorSets.Add(descriptorSet);
		++unusedSetIndex;
	}

	return descriptorSet;
}

void VulkanComputePipeline::ResetPipeline()
{
}
