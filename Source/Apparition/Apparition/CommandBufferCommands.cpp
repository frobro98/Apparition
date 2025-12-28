#include "CommandBufferCommands.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/HandleDefinitions.h"
#include "Internal/ImageFormatConversion.h"
#include "Internal/RenderOperationsConversion.h"
#include "Internal/VulkanInfos.h"

namespace Apparition
{
void BeginCommandBuffer(CommandBuffer commandBuffer, bool oneTimeSubmit)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	const u32 deviceIndex = GetDeviceIndexFromHandle(commandBuffer);
	DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
	u32 handleIndex = GetHandleIndex(commandBuffer);
	CommandBufferInternal& cbInternal = deviceInternal.commandBuffers[handleIndex - 1];

	VkCommandBufferBeginInfo beginInfo;
	Vk::ZeroInfoStruct(beginInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO);
	beginInfo.flags = oneTimeSubmit ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0;

	VkResult result = vkBeginCommandBuffer(cbInternal.commandBuffer, &beginInfo);
	CHECK_VK(result);

	cbInternal.hasBegun = true;
}

void EndCommandBuffer(CommandBuffer commandBuffer)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	const u32 deviceIndex = GetDeviceIndexFromHandle(commandBuffer);
	DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
	u32 handleIndex = GetHandleIndex(commandBuffer);
	CommandBufferInternal& cbInternal = deviceInternal.commandBuffers[handleIndex - 1];

	VkResult result = vkEndCommandBuffer(cbInternal.commandBuffer);
	CHECK_VK(result);
	cbInternal.hasBegun = false;
}

 void BeginRendering(CommandBuffer commandBuffer, const RenderSetupParams& renderSetupParams)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	const u32 deviceIndex = GetDeviceIndexFromHandle(commandBuffer);
	DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
	u32 handleIndex = GetHandleIndex(commandBuffer);
	CommandBufferInternal& cbInternal = deviceInternal.commandBuffers[handleIndex - 1];
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
		renderAttachementInfo.loadOp = ApparitionLoadToVkLoad(GetLoadOperation(colorAttachment.loadStoreOps));
		renderAttachementInfo.storeOp = ApparitionStoreToVkStore(GetStoreOperation(colorAttachment.loadStoreOps));

		u32 viewIndex = GetHandleIndex(colorAttachment.imageView);
		ImageViewResourceInternal& viewInternal = deviceInternal.imageViewResources[viewIndex];
		renderAttachementInfo.imageView = viewInternal.imageView;
		colorAttachments.Add(renderAttachementInfo);
	}

	// Depth attachment
	VkRenderingAttachmentInfo depthAttachment;
	Vk::ZeroInfoStruct(depthAttachment, VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO);
	if (renderSetupParams.depthAttachment.imageView.handle != InvalidHandle)
	{
		const RenderAttachment& renderDepthAttachment = renderSetupParams.depthAttachment;
		// TODO: There currently is not a dedicated 
		depthAttachment.clearValue.depthStencil = {
			renderDepthAttachment.clearValue.depthStencil.depth,
			renderDepthAttachment.clearValue.depthStencil.stencil
		};
		depthAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
		depthAttachment.loadOp = ApparitionLoadToVkLoad(GetLoadOperation(renderDepthAttachment.loadStoreOps));
		depthAttachment.storeOp = ApparitionStoreToVkStore(GetStoreOperation(renderDepthAttachment.loadStoreOps));

		u32 viewIndex = GetHandleIndex(renderDepthAttachment.imageView);
		ImageViewResourceInternal& viewInternal = deviceInternal.imageViewResources[viewIndex];
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
		stencilAttachment.loadOp = ApparitionLoadToVkLoad(GetLoadOperation(renderStencilAttachment.loadStoreOps));
		stencilAttachment.storeOp = ApparitionStoreToVkStore(GetStoreOperation(renderStencilAttachment.loadStoreOps));

		u32 viewIndex = GetHandleIndex(renderStencilAttachment.imageView);
		ImageViewResourceInternal& viewInternal = deviceInternal.imageViewResources[viewIndex];
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
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	const u32 deviceIndex = GetDeviceIndexFromHandle(commandBuffer);
	DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
	u32 handleIndex = GetHandleIndex(commandBuffer);
	CommandBufferInternal& cbInternal = deviceInternal.commandBuffers[handleIndex - 1];
	Assert(cbInternal.hasBegun);

	vkCmdEndRendering(cbInternal.commandBuffer);
}

void BindVertexBuffers(CommandBuffer commandBuffer, const BindVertexBufferDesc& desc)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	const u32 deviceIndex = GetDeviceIndexFromHandle(commandBuffer);
	const DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
	const u32 handleIndex = GetHandleIndex(commandBuffer);
	const CommandBufferInternal& cbInternal = deviceInternal.commandBuffers[handleIndex - 1];
	Assert(cbInternal.hasBegun);

	const u32 vbIndex = GetHandleIndex(desc.vertexBuffer);
	// TODO: THIS NEEDS TO BE SOME SORT OF FUNCTION THAT'S SPECIFIC TO EACH TYPE
	VkBuffer vbBuffer = deviceInternal.bufferResources[vbIndex - 1].buffer;

	const VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cbInternal.commandBuffer, 0, 1, &vbBuffer, offsets);
}

void BindIndexBuffer(CommandBuffer commandBuffer, const BindIndexBufferDesc& desc)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	const u32 deviceIndex = GetDeviceIndexFromHandle(commandBuffer);
	const DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
	const u32 handleIndex = GetHandleIndex(commandBuffer);
	const CommandBufferInternal& cbInternal = deviceInternal.commandBuffers[handleIndex - 1];
	Assert(cbInternal.hasBegun);

	const u32 ibIndex = GetHandleIndex(desc.indexBuffer);
	// TODO: THIS NEEDS TO BE SOME SORT OF FUNCTION THAT'S SPECIFIC TO EACH TYPE
	VkBuffer ibBuffer = deviceInternal.bufferResources[ibIndex - 1].buffer;

	// TODO: VK_INDEX_TYPE must be a consistent setting for index buffers. Vulkan will catch this issue, theoretically,
	// but we'd like to catch it in some way too
	vkCmdBindIndexBuffer(cbInternal.commandBuffer, ibBuffer, 0, VK_INDEX_TYPE_UINT16);
}

void SetViewportAndScissor(CommandBuffer commandBuffer, const ViewportDesc& viewDesc, const ScissorDesc& scissorDesc)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	const u32 deviceIndex = GetDeviceIndexFromHandle(commandBuffer);
	const DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
	const u32 handleIndex = GetHandleIndex(commandBuffer);
	const CommandBufferInternal& cbInternal = deviceInternal.commandBuffers[handleIndex - 1];
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

void DrawIndexed(CommandBuffer commandBuffer, u32 indexCount)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	const u32 deviceIndex = GetDeviceIndexFromHandle(commandBuffer);
	const DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
	const u32 handleIndex = GetHandleIndex(commandBuffer);
	const CommandBufferInternal& cbInternal = deviceInternal.commandBuffers[handleIndex - 1];
	Assert(cbInternal.hasBegun);

	vkCmdDrawIndexed(cbInternal.commandBuffer, indexCount, 1, 0, 0, 0);
}

void CopyBuffer(CommandBuffer commandBuffer, const BufferCopyDesc& copyDesc)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	const u32 deviceIndex = GetDeviceIndexFromHandle(commandBuffer);
	const DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
	const u32 handleIndex = GetHandleIndex(commandBuffer);
	const CommandBufferInternal& cbInternal = deviceInternal.commandBuffers[handleIndex - 1];
	Assert(cbInternal.hasBegun);

	const u32 srcBufferIndex = GetHandleIndex(copyDesc.srcBuffer);
	const u32 dstBufferIndex = GetHandleIndex(copyDesc.dstBuffer);
	// TODO: THIS NEEDS TO BE SOME SORT OF FUNCTION THAT'S SPECIFIC TO EACH TYPE
	VkBuffer vkSrcBuffer = deviceInternal.bufferResources[srcBufferIndex - 1].buffer;
	VkBuffer vkDstBuffer = deviceInternal.bufferResources[dstBufferIndex - 1].buffer;

	VkBufferCopy copyRegion{};
	copyRegion.size = copyDesc.size;
	copyRegion.srcOffset = copyDesc.srcOffset;
	copyRegion.dstOffset = copyDesc.dstOffset;
	vkCmdCopyBuffer(cbInternal.commandBuffer, vkSrcBuffer, vkDstBuffer, 1, &copyRegion);
}

void ImageMemoryBarrier(CommandBuffer commandBuffer, const ImageMemoryBarrierDesc& barrierDesc)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	const u32 deviceIndex = GetDeviceIndexFromHandle(commandBuffer);
	DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
	const u32 handleIndex = GetHandleIndex(commandBuffer);
	const CommandBufferInternal& cbInternal = deviceInternal.commandBuffers[handleIndex - 1];
	Assert(cbInternal.hasBegun);

	const u32 imageResourceIndex = GetHandleIndex(barrierDesc.image);
	ImageResourceInternal& imgInternal = deviceInternal.imageResources[imageResourceIndex];

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
