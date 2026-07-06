#include "CommandBufferCommands.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/Conversions.h"
#include "Internal/DeviceManager.h"
#include "Internal/HandleDefinitions.h"
#include "Internal/ImageFormatConversion.h"
#include "Internal/VulkanInfos.h"

namespace Apparition
{
void BeginCommandBuffer(CommandBuffer commandBuffer, bool oneTimeSubmit)
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

void EndCommandBuffer(CommandBuffer commandBuffer)
{
	CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	VkResult result = vkEndCommandBuffer(cbInternal.commandBuffer);
	CHECK_VK(result);
	cbInternal.hasBegun = false;
}

 void BeginRendering(CommandBuffer commandBuffer, const RenderSetupParams& renderSetupParams)
{
	 const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);
	
	DynamicArray<VkRenderingAttachmentInfo> colorAttachments;
	colorAttachments.Reserve(renderSetupParams.colorAttachments.Size());
	for (const RenderAttachment& colorAttachment : renderSetupParams.colorAttachments)
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
	if (renderSetupParams.depthAttachment.imageView.handle != InvalidHandle)
	{
		const RenderAttachment& renderDepthAttachment = renderSetupParams.depthAttachment;
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
	if (renderSetupParams.stencilAttachment.imageView.handle != InvalidHandle)
	{
		const RenderAttachment& renderStencilAttachment = renderSetupParams.stencilAttachment;
		// TODO: There currently is not a dedicated 
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

void EndRendering(CommandBuffer commandBuffer)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	vkCmdEndRendering(cbInternal.commandBuffer);
}

void BindVertexBuffers(CommandBuffer commandBuffer, const BindVertexBufferDesc& desc)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	VkBuffer vbBuffer = GetBufferInternal(desc.vertexBuffer).buffer;

	const VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cbInternal.commandBuffer, 0, 1, &vbBuffer, offsets);
}

void BindIndexBuffer(CommandBuffer commandBuffer, const BindIndexBufferDesc& desc)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	VkBuffer ibBuffer = GetBufferInternal(desc.indexBuffer).buffer;

	// TODO: VK_INDEX_TYPE must be a consistent setting for index buffers. Vulkan will catch this issue, theoretically,
	// but we'd like to catch it in some way too
	vkCmdBindIndexBuffer(cbInternal.commandBuffer, ibBuffer, 0, VK_INDEX_TYPE_UINT16);
}

void SetViewportAndScissor(CommandBuffer commandBuffer, const ViewportDesc& viewDesc, const ScissorDesc& scissorDesc)
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

APPARITION_API void BindGraphicsPipeline(CommandBuffer commandBuffer, Pipeline pipeline)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	VkPipeline vkPipeline = GetPipelineInternal(pipeline).pipeline;
	vkCmdBindPipeline(cbInternal.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);
}

void DrawIndexed(CommandBuffer commandBuffer, u32 indexCount)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	vkCmdDrawIndexed(cbInternal.commandBuffer, indexCount, 1, 0, 0, 0);
}

void CopyBuffer(CommandBuffer commandBuffer, const BufferCopyDesc& copyDesc)
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

void ImageMemoryBarrier(CommandBuffer commandBuffer, const ImageMemoryBarrierDesc& barrierDesc)
{
	const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
	Assert(cbInternal.hasBegun);

	ImageInternal& imgInternal = GetImageInternal(barrierDesc.image);

	// TODO: There is no validation between the access and the format
	VkImageMemoryBarrier2 imageBarrier;
	Vk::ZeroInfoStruct(imageBarrier, VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2);
	imageBarrier.oldLayout = ApparitionImageAccessToVkLayout(imgInternal.access);
	imageBarrier.newLayout = ApparitionImageAccessToVkLayout(barrierDesc.access);
	imageBarrier.srcAccessMask = ApparitionImageAccessToAccessMask(imgInternal.access);
	imageBarrier.dstAccessMask = ApparitionImageAccessToAccessMask(barrierDesc.access);
	imageBarrier.srcStageMask = ApparitionImageAccessToPipelineStage(imgInternal.access);
	imageBarrier.dstStageMask = ApparitionImageAccessToPipelineStage(barrierDesc.access);
	imageBarrier.image = imgInternal.image;
	// TODO: Will need to make this part of the barrier desc. Can be a default most of the time
	imageBarrier.subresourceRange.aspectMask = ApparitionImageViewAspectToVkAspectFlags(barrierDesc.aspect);
	imageBarrier.subresourceRange.layerCount = 1;
	imageBarrier.subresourceRange.levelCount = 1;
	imageBarrier.subresourceRange.baseMipLevel = 0;
	imageBarrier.subresourceRange.baseArrayLayer = 0;

	VkDependencyInfo dependencyInfo;
	Vk::ZeroInfoStruct(dependencyInfo, VK_STRUCTURE_TYPE_DEPENDENCY_INFO);
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &imageBarrier;

	vkCmdPipelineBarrier2(cbInternal.commandBuffer, &dependencyInfo);
}
}
