// Copyright 2020, Nathan Blane

#include "Utilities/Array.hpp"
#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorPool.h"
#include "VulkanRenderPass.h"
#include "VulkanShaders.h"
#include "VulkanCommandBuffer.h"
#include "VulkanUtilities.h"
#include "VulkanFence.hpp"

#include "Debugging/DebugOutput.hpp"


VulkanPipelineLayout::VulkanPipelineLayout(const VulkanDevice& device, DynamicArray<VulkanDescriptorSetLayout*>&& layouts)
	: descriptorSetLayouts(MOVE(layouts)),
	logicalDevice(&device)
{
	DynamicArray<VkDescriptorSetLayout> layoutHandles;
	layoutHandles.Reserve(descriptorSetLayouts.Size());
	// TODO - Make arrays for each loop able
	for (u32 i = 0; i < descriptorSetLayouts.Size(); ++i)
	{
		VkDescriptorSetLayout handle = descriptorSetLayouts[i]->GetNativeHandle();
		Assert(handle != VK_NULL_HANDLE);
		layoutHandles.Add(handle);
	}

	VkPipelineLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.setLayoutCount = layoutHandles.Size();
	layoutInfo.pSetLayouts = layoutHandles.GetData();

	NOT_USED VkResult result = vkCreatePipelineLayout(logicalDevice->GetNativeHandle(), &layoutInfo, nullptr, &pipelineLayout);
	CHECK_VK(result);
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
	vkDestroyPipelineLayout(logicalDevice->GetNativeHandle(), pipelineLayout, nullptr);
}

VulkanPipeline::VulkanPipeline(const VulkanDevice& device)
	: logicalDevice(&device)
{
}

VulkanPipeline::~VulkanPipeline()
{
	delete pipelineLayout;
	if (pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(logicalDevice->GetNativeHandle(), pipeline, nullptr);
	}
}

void SetupInputs(GraphicsPipelineDescription& initInfo, DynamicArray<VkVertexInputBindingDescription>& inputBindings, DynamicArray<VkVertexInputAttributeDescription>& inputAttribs)
{
	VertexInputDescriptionList& inputs = initInfo.vertexInputs;
	u32 count = inputs.vertexInputCount;

	if (count > 0)
	{
		u32 stride = inputs.inputDescs[0].stride;
		u32 currentHighestBinding = 0;
		for (u32 i = 0; i < count; ++i)
		{
			VkVertexInputAttributeDescription desc = {};
			desc.location = inputs.inputDescs[i].location;
			desc.binding = inputs.inputDescs[i].binding;
			desc.format = MusaInputsToVk(inputs.inputDescs[i].type);
			desc.offset = inputs.inputDescs[i].offset;

			if (desc.binding > currentHighestBinding)
			{
				currentHighestBinding = desc.binding;
			}

			inputAttribs.Add(desc);
		}

		for (u32 i = 0; i < currentHighestBinding + 1; ++i)
		{
			VkVertexInputBindingDescription desc = {};
			desc.binding = i;
			desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			desc.stride = stride;

			inputBindings.Add(desc);
		}
	}
}

void VulkanPipeline::Initialize(const VulkanPipelineLayout* layout, const GraphicsPipelineDescription& initInfo, VulkanRenderPass* renderPass)
{
	pipelineLayout = layout;
	desc = initInfo;

	DynamicArray<VkPipelineShaderStageCreateInfo> shaderStages;

	const VulkanVertexShader& vertexShader = *reinterpret_cast<const VulkanVertexShader*>(initInfo.vertexShader);
	const VulkanFragmentShader& fragmentShader = *reinterpret_cast<const VulkanFragmentShader*>(initInfo.fragmentShader);

	VkPipelineShaderStageCreateInfo vkShaderStage = {};
	vkShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vkShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vkShaderStage.module = vertexShader.shaderData.module;
	vkShaderStage.pName = "main";
	vkShaderStage.pSpecializationInfo;
	shaderStages.Add(vkShaderStage);

	if (initInfo.fragmentShader != nullptr)
	{
		vkShaderStage = {};
		vkShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vkShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		vkShaderStage.module = fragmentShader.shaderData.module;
		vkShaderStage.pName = "main";
		vkShaderStage.pSpecializationInfo;
		shaderStages.Add(vkShaderStage);
	}

	DynamicArray<VkVertexInputBindingDescription> inputBindings;
	DynamicArray<VkVertexInputAttributeDescription> inputAttribs;
	SetupInputs(desc, inputBindings, inputAttribs);

	// Input information
	VkPipelineVertexInputStateCreateInfo vertInputInfo = {};
	vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertInputInfo.vertexBindingDescriptionCount = inputBindings.Size();
	vertInputInfo.pVertexBindingDescriptions = inputBindings.GetData();
	vertInputInfo.vertexAttributeDescriptionCount = inputAttribs.Size();
	vertInputInfo.pVertexAttributeDescriptions = inputAttribs.GetData();

	// Input Assembly info
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology = MusaTopologyToVk(desc.topology);
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	// Rasterizer info
	VkPipelineRasterizationStateCreateInfo rasterizerInfo = {};
	rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerInfo.polygonMode = MusaPolyToVk(desc.rasterizerDesc.fillMode);
	rasterizerInfo.cullMode = MusaCullToVk(desc.rasterizerDesc.cullMode);
	rasterizerInfo.depthBiasEnable = desc.rasterizerDesc.depthBiasConstant > 0;
	rasterizerInfo.depthBiasConstantFactor = desc.rasterizerDesc.depthBiasConstant;
	rasterizerInfo.depthBiasSlopeFactor = desc.rasterizerDesc.depthBiasSlope;
	rasterizerInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizerInfo.lineWidth = 1.f;

	// Multisampling info
	VkPipelineMultisampleStateCreateInfo multisampleInfo = {};
	multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	// TODO - Look up what these sampling settings are meant to do in the render pipeline
	//multisampleInfo.sampleShadingEnable = VK_FALSE;
	multisampleInfo.rasterizationSamples = renderPass->GetAttachments()[0].samples;
	//multisampleInfo.alphaToOneEnable = VK_FALSE;
	//multisampleInfo.alphaToCoverageEnable = VK_FALSE;
	//multisampleInfo.minSampleShading = 0.f;
	//multisampleInfo.pSampleMask = nullptr;

	// Depth-stencil info
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
	depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilInfo.depthTestEnable = desc.depthStencilTestDesc.depthCompareOp != CompareOperation::None;
	// TODO - This isn't the greatest. Might be better to check if the depth texture is readonly and if not, and depth test enabled, set to true!
	depthStencilInfo.depthWriteEnable = desc.depthStencilTestDesc.depthWriteEnabled;
	depthStencilInfo.stencilTestEnable = desc.depthStencilTestDesc.frontStencilTestEnabled || desc.depthStencilTestDesc.backStencilTestEnabled;
	depthStencilInfo.depthCompareOp = MusaCompareOpToVk(desc.depthStencilTestDesc.depthCompareOp);

	// Blending states
	DynamicArray<VkPipelineColorBlendAttachmentState> colorBlendAttachments(desc.colorAttachments.Size());
	for (u32 i = 0; i < colorBlendAttachments.Size(); ++i)
	{
		BlendingDescription& blendDesc = desc.blendingDescs[i];
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.blendEnable = blendDesc.colorBlendOperation != BlendOperation::None ||
											blendDesc.alphaBlendOperation != BlendOperation::None;
		colorBlendAttachment.colorBlendOp = MusaBlendOpToVk(blendDesc.colorBlendOperation);
		colorBlendAttachment.alphaBlendOp = MusaBlendOpToVk(blendDesc.alphaBlendOperation);
		colorBlendAttachment.colorWriteMask = (VkColorComponentFlags)blendDesc.colorMask;
		colorBlendAttachment.srcColorBlendFactor = MusaBlendFactorToVk(blendDesc.srcBlend);
		colorBlendAttachment.dstColorBlendFactor = MusaBlendFactorToVk(blendDesc.dstBlend);
		colorBlendAttachment.srcAlphaBlendFactor = MusaBlendFactorToVk(blendDesc.srcAlphaBlend);
		colorBlendAttachment.dstAlphaBlendFactor = MusaBlendFactorToVk(blendDesc.dstAlphaBlend);

		colorBlendAttachments[i] = colorBlendAttachment;
	}

	// Color blend state info
	VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
	colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendInfo.attachmentCount = (u32)colorBlendAttachments.Size();
	colorBlendInfo.pAttachments = colorBlendAttachments.GetData();
	colorBlendInfo.blendConstants[0] = 1.0f;
	colorBlendInfo.blendConstants[1] = 1.0f;
	colorBlendInfo.blendConstants[2] = 1.0f;
	colorBlendInfo.blendConstants[3] = 1.0f;

	// Viewport info
	VkPipelineViewportStateCreateInfo viewportInfo = {};
	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportInfo.viewportCount = 1;
	viewportInfo.scissorCount = 1;

	// Dynamic states
	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	// Dynamic state info
	VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.dynamicStateCount = (u32)ArraySize(dynamicStates);
	dynamicStateInfo.pDynamicStates = dynamicStates;

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = shaderStages.Size();
	pipelineInfo.pStages = shaderStages.GetData();
	pipelineInfo.pVertexInputState = &vertInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pViewportState = &viewportInfo;
	pipelineInfo.pRasterizationState = &rasterizerInfo;
	pipelineInfo.pMultisampleState = &multisampleInfo;
	pipelineInfo.pDepthStencilState = &depthStencilInfo;
	pipelineInfo.pColorBlendState = &colorBlendInfo;
	pipelineInfo.pDynamicState = &dynamicStateInfo;
	pipelineInfo.layout = pipelineLayout->GetNativeHandle();
	pipelineInfo.renderPass = renderPass->GetNativeHandle();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	NOT_USED VkResult result = vkCreateGraphicsPipelines(logicalDevice->GetNativeHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
	CHECK_VK(result);

	writeDescriptorSet.SetupWriteDescriptors(*layout->GetDescriptorSetLayouts()[0]);
}

void VulkanPipeline::Bind(VulkanCommandBuffer * cmdBuffer) const
{
	if (cmdBuffer->GetLevel() == VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		Assert(cmdBuffer->IsInRenderPass());
	}
	vkCmdBindPipeline(cmdBuffer->GetNativeHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void VulkanPipeline::BindDescriptorSet(VulkanCommandBuffer* cmdBuffer, VulkanDescriptorSet* descriptorSet) const
{
	if (cmdBuffer->GetLevel() == VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		Assert(cmdBuffer->IsInRenderPass());
	}

	VkDescriptorSet ds = descriptorSet->GetNativeHandle();
	vkCmdBindDescriptorSets(cmdBuffer->GetNativeHandle(), 
		VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout->GetNativeHandle(),
		0, 1, &ds, 0, nullptr);
}

VulkanDescriptorSet* VulkanPipeline::GetUnusedDescriptorSet(VulkanCommandBuffer& cmdBuffer)
{
	std::lock_guard<std::mutex> lock(descriptorMut);
	VulkanDescriptorSet* descriptorSet = nullptr;
	for (auto set : descriptorSets)
	{
		if (set->associatedCommandBuffer == &cmdBuffer)
		{
			VulkanFence* fence = cmdBuffer.GetFence();
			if (set->associatedFenceSignal < fence->GetFenceSignaledCount())
			{
				descriptorSet = set;
				set->associatedFenceSignal = fence->GetFenceSignaledCount();
				break;
			}
		}
	}

	if(descriptorSet == nullptr)
	{
		VulkanDescriptorPool* pool = logicalDevice->GetActivePool();
		descriptorSet = pool->AllocateDescriptorSet(pipelineLayout->GetDescriptorSetLayouts()[0]);

		Assert(descriptorSet != nullptr);
		descriptorSets.Add(descriptorSet);
		descriptorSet->associatedCommandBuffer = &cmdBuffer;
		descriptorSet->associatedFenceSignal = cmdBuffer.GetFence()->GetFenceSignaledCount();
	}

	return descriptorSet;
}
