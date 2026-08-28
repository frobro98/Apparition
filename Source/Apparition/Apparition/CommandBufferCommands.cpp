#include "CommandBufferCommands.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/Conversions.h"
#include "Internal/DeviceManager.h"
#include "Internal/HandleDefinitions.h"
#include "Internal/ImageFormatConversion.h"
#include "Internal/VulkanInfos.h"

namespace
{
using namespace Apparition;

// Create VkPipelineLayout
VkPipelineLayout CreatePipelineLayout(const AptnPipelineDescription& desc, VkDevice device)
{
	DynamicArray<VkDescriptorSetLayout> setLayouts;
	setLayouts.Reserve(desc.descriptorSets.Size());
	for (const AptnDescriptorSetLayout& layoutHandle : desc.descriptorSets)
	{
		DescriptorSetLayoutInternal& layoutInternal = GetDescriptorSetLayoutInternal(layoutHandle);
		setLayouts.Add(layoutInternal.descriptorSetLayout);
	}

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = setLayouts.Size();
	pipelineLayoutInfo.pSetLayouts = setLayouts.GetData();
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
	const VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
	CHECK_VK(result);

	return pipelineLayout;
}
}

namespace Apparition
{
void BeginCommandBuffer(AptnCommandBuffer commandBuffer, bool oneTimeSubmit)
{
	CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(!cbInternal.hasBegun);

	VkCommandBufferBeginInfo beginInfo;
	Vk::ZeroInfoStruct(beginInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO);
	beginInfo.flags = oneTimeSubmit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0;

	VkResult result = vkBeginCommandBuffer(cbInternal.commandBuffer, &beginInfo);
	CHECK_VK(result);
	cbInternal.hasBegun = true;
}

void EndCommandBuffer(AptnCommandBuffer commandBuffer)
{
	CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	VkResult result = vkEndCommandBuffer(cbInternal.commandBuffer);
	CHECK_VK(result);
	cbInternal.hasBegun = false;
}

 void BeginRendering(AptnCommandBuffer commandBuffer, const AptnRenderSetupParams& renderSetupParams)
{
	 const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);
	
	DynamicArray<VkRenderingAttachmentInfo> colorAttachments;
	colorAttachments.Reserve(renderSetupParams.colorAttachments.Size());
	for (const AptnRenderAttachment& colorAttachment : renderSetupParams.colorAttachments)
	{
		VkRenderingAttachmentInfo renderAttachementInfo;
		Vk::ZeroInfoStruct(renderAttachementInfo, VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO);
		renderAttachementInfo.clearValue.color = VkClearColorValue{ {
			colorAttachment.clearValue.color.r,
			colorAttachment.clearValue.color.g,
			colorAttachment.clearValue.color.b,
			colorAttachment.clearValue.color.a } };
		renderAttachementInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		renderAttachementInfo.loadOp = ApparitionLoadToVkLoad(LoadOperationFrom(colorAttachment.loadStoreOps));
		renderAttachementInfo.storeOp = ApparitionStoreToVkStore(StoreOperationFrom(colorAttachment.loadStoreOps));

		ImageViewInternal& viewInternal = GetImageViewInternal(colorAttachment.imageView);
		renderAttachementInfo.imageView = viewInternal.imageView;
		colorAttachments.Add(renderAttachementInfo);
	}

	// Depth attachment
	VkRenderingAttachmentInfo depthAttachment;
	Vk::ZeroInfoStruct(depthAttachment, VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO);
	if (IsValid(renderSetupParams.depthAttachment.imageView))
	{
		const AptnRenderAttachment& renderDepthAttachment = renderSetupParams.depthAttachment;
		depthAttachment.clearValue.depthStencil = {
			renderDepthAttachment.clearValue.depthStencil.depth,
			renderDepthAttachment.clearValue.depthStencil.stencil
		};
		depthAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		depthAttachment.loadOp = ApparitionLoadToVkLoad(LoadOperationFrom(renderDepthAttachment.loadStoreOps));
		depthAttachment.storeOp = ApparitionStoreToVkStore(StoreOperationFrom(renderDepthAttachment.loadStoreOps));

		ImageViewInternal& viewInternal = GetImageViewInternal(renderDepthAttachment.imageView);
		depthAttachment.imageView = viewInternal.imageView;
	}

	// Stencil attachment
	VkRenderingAttachmentInfo stencilAttachment;
	Vk::ZeroInfoStruct(stencilAttachment, VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO);
	if (IsValid(renderSetupParams.stencilAttachment.imageView))
	{
		const AptnRenderAttachment& renderStencilAttachment = renderSetupParams.stencilAttachment;
		stencilAttachment.clearValue.depthStencil = {
			renderStencilAttachment.clearValue.depthStencil.depth,
			renderStencilAttachment.clearValue.depthStencil.stencil
		};
		stencilAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		stencilAttachment.loadOp = ApparitionLoadToVkLoad(LoadOperationFrom(renderStencilAttachment.loadStoreOps));
		stencilAttachment.storeOp = ApparitionStoreToVkStore(StoreOperationFrom(renderStencilAttachment.loadStoreOps));

		ImageViewInternal& viewInternal = GetImageViewInternal(renderStencilAttachment.imageView);
		stencilAttachment.imageView = viewInternal.imageView;
	}
	else if (depthAttachment.imageView != VK_NULL_HANDLE)
	{
		stencilAttachment = depthAttachment;
	}

	VkRenderingInfo renderingInfo;
	Vk::ZeroInfoStruct(renderingInfo, VK_STRUCTURE_TYPE_RENDERING_INFO);
	renderingInfo.renderArea.extent = { renderSetupParams.renderWidth, renderSetupParams.renderHeight };
	renderingInfo.renderArea.offset = { 0, 0 };
	renderingInfo.layerCount = 1;
	//renderingInfo.viewMask;
	renderingInfo.colorAttachmentCount = colorAttachments.Size();
	renderingInfo.pColorAttachments = colorAttachments.GetData();
	if (depthAttachment.imageView != VK_NULL_HANDLE)
	{
		renderingInfo.pDepthAttachment = &depthAttachment;
	}
	if (stencilAttachment.imageView != VK_NULL_HANDLE)
	{
		renderingInfo.pStencilAttachment = &stencilAttachment;
	}

	vkCmdBeginRendering(cbInternal.commandBuffer, &renderingInfo);
}

void EndRendering(AptnCommandBuffer commandBuffer)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	vkCmdEndRendering(cbInternal.commandBuffer);
}

void BindVertexBuffers(AptnCommandBuffer commandBuffer, const AptnBindVertexBufferDesc& desc)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	VkBuffer vbBuffer = GetBufferInternal(desc.vertexBuffer).buffer;

	const VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cbInternal.commandBuffer, 0, 1, &vbBuffer, offsets);
}

void BindIndexBuffer(AptnCommandBuffer commandBuffer, const AptnBindIndexBufferDesc& desc)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	VkBuffer ibBuffer = GetBufferInternal(desc.indexBuffer).buffer;

	// TODO: VK_INDEX_TYPE must be a consistent setting for index buffers. Vulkan will catch this issue, theoretically,
	// but we'd like to catch it in some way too
	vkCmdBindIndexBuffer(cbInternal.commandBuffer, ibBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void SetViewportAndScissor(AptnCommandBuffer commandBuffer, const AptnViewportDesc& viewDesc, const AptnScissorDesc& scissorDesc)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	VkViewport viewport = {
		.x = viewDesc.x,
		.y = viewDesc.y,
		.width = viewDesc.width,
		.height = viewDesc.height,
		.minDepth = 0.f,
		.maxDepth = 1.f
	};
	vkCmdSetViewport(cbInternal.commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = {
		.offset = { scissorDesc.offsetX, scissorDesc.offsetY },
		.extent = { scissorDesc.extentX, scissorDesc.extentY }
	};
	vkCmdSetScissor(cbInternal.commandBuffer, 0, 1, &scissor);
}

void BindGraphicsPipeline(AptnCommandBuffer commandBuffer, AptnPipeline pipeline)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	VkPipeline vkPipeline = GetPipelineInternal(pipeline).pipeline;
	vkCmdBindPipeline(cbInternal.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
}

void BindDescriptorSets(AptnCommandBuffer commandBuffer, const AptnBindDescriptorSetsDesc& bindDescriptorSetsDesc)
{
	const DeviceInternal& deviceInternal = GetDeviceInternal(commandBuffer);
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	DynamicArray<VkDescriptorSet> setHandles;
	setHandles.Reserve(bindDescriptorSetsDesc.descriptorSets.Size());
	for (const AptnDescriptorSet& descriptorSet : bindDescriptorSetsDesc.descriptorSets)
	{
		DescriptorSetInternal& setInternal = GetDescriptorSetInternal(descriptorSet);
		setHandles.Add(setInternal.descriptorSet);
	}
	// Create VkPipelineLayout
	VkPipelineLayout pipelineLayout = CreatePipelineLayout(bindDescriptorSetsDesc.pipelineDesc, deviceInternal.device);

	VkPipelineBindPoint bindPoint = bindDescriptorSetsDesc.bindPoint == AptnBindPoint::Graphics ? VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE;

	vkCmdBindDescriptorSets(cbInternal.commandBuffer, bindPoint, pipelineLayout, bindDescriptorSetsDesc.firstSet, setHandles.Size(), setHandles.GetData(), 0, nullptr);

	vkDestroyPipelineLayout(deviceInternal.device, pipelineLayout, nullptr);
}

void BindSamplerHeap(AptnCommandBuffer commandBuffer, AptnSamplerHeap samplerHeap)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	const SamplerHeapInternal samplerHeapInternal = GetSamplerHeapInternal(samplerHeap);

	VkBindHeapInfoEXT bindSamplerHeapInfo
	{
		.sType = VK_STRUCTURE_TYPE_BIND_HEAP_INFO_EXT,
		.heapRange
		{
			.address = samplerHeapInternal.heapDeviceAddress,
			.size = samplerHeapInternal.heapSize
		},
		.reservedRangeOffset = samplerHeapInternal.heapSize - samplerHeapInternal.heapReservedRange,
		.reservedRangeSize = samplerHeapInternal.heapReservedRange
	};
	vkCmdBindSamplerHeapEXT(cbInternal.commandBuffer, &bindSamplerHeapInfo);
}

void BindResourceHeap(AptnCommandBuffer commandBuffer, AptnResourceHeap resourceHeap)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	const ResourceHeapInternal resourceHeapInternal = GetResourceHeapInternal(resourceHeap);

	VkBindHeapInfoEXT bindSamplerHeapInfo
	{
		.sType = VK_STRUCTURE_TYPE_BIND_HEAP_INFO_EXT,
		.heapRange
		{
			.address = resourceHeapInternal.heapDeviceAddress,
			.size = resourceHeapInternal.heapSize
		},
		.reservedRangeOffset = resourceHeapInternal.heapSize - resourceHeapInternal.heapReservedRange,
		.reservedRangeSize = resourceHeapInternal.heapReservedRange
	};
	vkCmdBindResourceHeapEXT(cbInternal.commandBuffer, &bindSamplerHeapInfo);
}

void PushData(AptnCommandBuffer commandBuffer, const AptnPushDataDesc& pushData)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	VkPushDataInfoEXT pushDataInfo
	{
		.sType = VK_STRUCTURE_TYPE_PUSH_DATA_INFO_EXT,
		.data = {.address = pushData.dataAddress, .size = pushData.dataSize }
	};
	vkCmdPushDataEXT(cbInternal.commandBuffer, &pushDataInfo);
}

void DrawIndexed(AptnCommandBuffer commandBuffer, u32 indexCount, u32 firstIndex, u32 instanceCount, u32 firstInstance)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	vkCmdDrawIndexed(cbInternal.commandBuffer, indexCount, instanceCount, firstIndex, 0, firstInstance);
}

void CopyBuffer(AptnCommandBuffer commandBuffer, const AptnBufferCopyDesc& copyDesc)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	VkBuffer vkSrcBuffer = GetBufferInternal(copyDesc.srcBuffer).buffer;
	VkBuffer vkDstBuffer = GetBufferInternal(copyDesc.dstBuffer).buffer;

	VkBufferCopy copyRegion{};
	copyRegion.size = copyDesc.size;
	copyRegion.srcOffset = copyDesc.srcOffset;
	copyRegion.dstOffset = copyDesc.dstOffset;
	vkCmdCopyBuffer(cbInternal.commandBuffer, vkSrcBuffer, vkDstBuffer, 1, &copyRegion);
}

void CopyBufferToImage(AptnCommandBuffer commandBuffer, const AptnBufferToImageCopyDesc& copyDesc)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	VkBuffer vkSrcBuffer = GetBufferInternal(copyDesc.srcBuffer).buffer;
	VkImage vkDstImage = GetImageInternal(copyDesc.dstImage).image;

	const AptnBufferToImageCopyOutline& outline = copyDesc.outline;
	VkBufferImageCopy bufferCopyRegion
	{
		.bufferOffset = copyDesc.outline.bufferOffset,
		.imageSubresource
		{
			.aspectMask = ApparitionImageAspectToVk(outline.aspect),
			.mipLevel = outline.mipLevel,
			.baseArrayLayer = 0,
			.layerCount = 1
		},
		.imageExtent
		{
			.width = outline.imgWidth,
			.height = outline.imgHeight,
			.depth = 1
		}
	};
	vkCmdCopyBufferToImage(cbInternal.commandBuffer, vkSrcBuffer, vkDstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);
}

void CopyBufferRegionsToImage(AptnCommandBuffer commandBuffer, const AptnBufferRegionsToImageCopyDesc& copyRegions)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	VkBuffer vkSrcBuffer = GetBufferInternal(copyRegions.srcBuffer).buffer;
	VkImage vkDstImage = GetImageInternal(copyRegions.dstImage).image;

	const DynamicArray<AptnBufferToImageCopyOutline>& outlines = copyRegions.outlines;
	DynamicArray<VkBufferImageCopy> bufferCopyRegions;
	bufferCopyRegions.Reserve(outlines.Size());
	for (const AptnBufferToImageCopyOutline& outline : outlines)
	{
		VkBufferImageCopy bufferCopyRegion
		{
			.bufferOffset = outline.bufferOffset,
			.imageSubresource
			{
				.aspectMask = ApparitionImageAspectToVk(outline.aspect),
				.mipLevel = outline.mipLevel,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.imageExtent
			{
				.width = outline.imgWidth,
				.height = outline.imgHeight,
				.depth = 1
			}
		};
		bufferCopyRegions.Add(bufferCopyRegion);
	}
	vkCmdCopyBufferToImage(cbInternal.commandBuffer, vkSrcBuffer, vkDstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.Size(), bufferCopyRegions.GetData());
}

void BlitImage(AptnCommandBuffer commandBuffer, const AptnBlitImageDesc& blitDesc)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	NOT_USED VkImage srcImage = GetImageInternal(blitDesc.srcImage).image;
	NOT_USED VkImage dstImage = GetImageInternal(blitDesc.dstImage).image;

	//vkCmdBlitImage(cbInternal.commandBuffer, srcImage, srcAccessLayout, dstImage, dstAccessLayout, , filter);
}

void ImageMemoryBarrier(AptnCommandBuffer commandBuffer, const AptnImageMemoryBarrierDesc& barrierDesc)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	ImageInternal& imgInternal = GetImageInternal(barrierDesc.image);

	VkImageSubresourceRange subresourceRange{
		.aspectMask = ApparitionImageAspectToVk(barrierDesc.aspect),
		.baseMipLevel = barrierDesc.baseMipLevel,
		.levelCount = barrierDesc.mipLevelCount,
		.layerCount = 1
	};

	Assert(imgInternal.access.IsIndexValid(barrierDesc.baseMipLevel));
	AptnImageAccess imgAccess = imgInternal.access[barrierDesc.baseMipLevel];

	VkImageMemoryBarrier2 imageBarrier;
	Vk::ZeroInfoStruct(imageBarrier, VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2);
	imageBarrier.oldLayout = ApparitionImageAccessToVkLayout(imgAccess);
	imageBarrier.newLayout = ApparitionImageAccessToVkLayout(barrierDesc.access);
	imageBarrier.srcAccessMask = ApparitionImageAccessToVkAccess(imgAccess);
	imageBarrier.dstAccessMask = ApparitionImageAccessToVkAccess(barrierDesc.access);
	// TODO - Expose stage mask, since this kind of assumption is a little much for this kind of API
	imageBarrier.srcStageMask = ApparitionImageAccessToPipelineStage(imgAccess);
	imageBarrier.dstStageMask = ApparitionImageAccessToPipelineStage(barrierDesc.access);
	imageBarrier.image = imgInternal.image;
	imageBarrier.subresourceRange = subresourceRange;

	VkDependencyInfo dependencyInfo;
	Vk::ZeroInfoStruct(dependencyInfo, VK_STRUCTURE_TYPE_DEPENDENCY_INFO);
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &imageBarrier;

	vkCmdPipelineBarrier2(cbInternal.commandBuffer, &dependencyInfo);

	const u32 mipLevelsToChange = barrierDesc.baseMipLevel + barrierDesc.mipLevelCount;
	for (u32 i = barrierDesc.baseMipLevel; i < mipLevelsToChange; ++i)
	{
		imgInternal.access[i] = barrierDesc.access;
	}
}
}
