// Copyright 2020, Nathan Blane

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"
#include "VulkanFence.hpp"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanBufferAllocation.hpp"

#include "Debugging/Assertion.hpp"

WALL_WRN_PUSH
#include "fmt/format.h"
WALL_WRN_POP


VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice& device, VulkanCommandBufferManager& manager)
	:logicalDevice(&device),
	cmdBufferManager(&manager),
	state(CommandBufferState::Uninitialized)
{
}

void VulkanCommandBuffer::Initialize(VkCommandBufferLevel cmdBufferLevel, VkCommandPool cmdPool)
{
	bufferLevel = cmdBufferLevel;

	VkCommandBufferAllocateInfo cmdAllocInfo = {};
	cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdAllocInfo.commandBufferCount = 1;
	cmdAllocInfo.commandPool = cmdPool;
	cmdAllocInfo.level = cmdBufferLevel;

	NOT_USED VkResult result = vkAllocateCommandBuffers(logicalDevice->GetNativeHandle(), &cmdAllocInfo, &commandBuffer);
	CHECK_VK(result);

	state = CommandBufferState::Initialized;

	fence = logicalDevice->GetFenceManager().CreateFence();
}

void VulkanCommandBuffer::Initialize(VkCommandBuffer cmdBuffer, VkCommandBufferLevel cmdBufferLevel)
{
	Assert(cmdBuffer != VK_NULL_HANDLE);

	bufferLevel = cmdBufferLevel;

	commandBuffer = cmdBuffer;
	state = CommandBufferState::Initialized;
	fence = logicalDevice->GetFenceManager().CreateFence();
}

void VulkanCommandBuffer::DeInitialize(VkCommandPool cmdPool)
{
	Assert(commandBuffer != VK_NULL_HANDLE);

	vkFreeCommandBuffers(logicalDevice->GetNativeHandle(), cmdPool, 1, &commandBuffer);
	commandBuffer = VK_NULL_HANDLE;
	state = CommandBufferState::Uninitialized;
}

void VulkanCommandBuffer::Begin(VkCommandBufferUsageFlags cbUsageFlags, const VkCommandBufferInheritanceInfo* pInheritanceInfo)
{
	Assert(state == CommandBufferState::Initialized || state == CommandBufferState::Submitted);
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = cbUsageFlags;
	// TODO - If command buffer usage flags are specific values, parts of the inheritance info must be valid. Therefore, these values must be checked...
	beginInfo.pInheritanceInfo = pInheritanceInfo;
	NOT_USED VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
	CHECK_VK(result);
	state = CommandBufferState::Began;
}

void VulkanCommandBuffer::End()
{
	Assert(state == CommandBufferState::Began);
	NOT_USED VkResult result = vkEndCommandBuffer(commandBuffer);
	CHECK_VK(result);
	state = CommandBufferState::PendingSubmit;
}

void VulkanCommandBuffer::BeginRenderpass(VulkanFramebuffer* frameBuffer, const DynamicArray<VkClearValue>& clearColors)
{
	if (state == CommandBufferState::Submitted ||
		state == CommandBufferState::Initialized)
	{
		Begin();
	}
	Assert(state == CommandBufferState::Began);

	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = frameBuffer->GetRenderPass()->GetNativeHandle();
	renderPassBeginInfo.framebuffer = frameBuffer->GetNativeHandle();
	renderPassBeginInfo.renderArea.offset = { 0,0 };
	renderPassBeginInfo.renderArea.extent.width = frameBuffer->GetWidth();
	renderPassBeginInfo.renderArea.extent.height = frameBuffer->GetHeight();
	renderPassBeginInfo.clearValueCount = clearColors.Size();
	renderPassBeginInfo.pClearValues = clearColors.GetData();

	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	state = CommandBufferState::InRenderPass;
}

void VulkanCommandBuffer::EndRenderPass()
{
	if (state == CommandBufferState::InRenderPass)
	{
		vkCmdEndRenderPass(commandBuffer);
		state = CommandBufferState::Began;
	}
}

void VulkanCommandBuffer::ImageMemoryBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, u32 imageMemoryBarrierCount, const VkImageMemoryBarrier * pImageMemoryBarriers)
{
	if (state == CommandBufferState::Submitted ||
		state == CommandBufferState::Initialized)
	{
		Begin();
	}
	Assert(state == CommandBufferState::Began ||
		   state == CommandBufferState::InRenderPass);

	vkCmdPipelineBarrier(
		commandBuffer,
		srcStageMask, dstStageMask,
		dependencyFlags, 
		0, nullptr,
		0, nullptr,
		imageMemoryBarrierCount, pImageMemoryBarriers
	);
}

void VulkanCommandBuffer::BufferMemoryBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, u32 bufferMemoryBarrierCount, const VkBufferMemoryBarrier * pBufferMemoryBarriers)
{
	if (state == CommandBufferState::Submitted ||
		state == CommandBufferState::Initialized)
	{
		Begin();
	}
	Assert(state == CommandBufferState::Began ||
		state == CommandBufferState::InRenderPass);

	vkCmdPipelineBarrier(commandBuffer,
		srcStageMask, dstStageMask, 
		dependencyFlags, 
		0, nullptr,
		bufferMemoryBarrierCount, pBufferMemoryBarriers,
		0, nullptr);
}

void VulkanCommandBuffer::MemoryBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, u32 memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers)
{
	if (state == CommandBufferState::Submitted ||
		state == CommandBufferState::Initialized)
	{
		Begin();
	}
	Assert(state == CommandBufferState::Began ||
		state == CommandBufferState::InRenderPass);

	vkCmdPipelineBarrier(commandBuffer,
		srcStageMask, dstStageMask,
		dependencyFlags,
		memoryBarrierCount, pMemoryBarriers,
		0, nullptr,
		0, nullptr
	);
}

void VulkanCommandBuffer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, u32 regionCount, const VkBufferCopy * pRegions)
{
	if (state == CommandBufferState::Submitted ||
		state == CommandBufferState::Initialized)
	{
		Begin();
	}

	Assert(state == CommandBufferState::Began ||
		state == CommandBufferState::InRenderPass);

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
}

void VulkanCommandBuffer::CopyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstLayout, u32 regionCount, const VkBufferImageCopy * pRegions)
{
	if (state == CommandBufferState::Submitted ||
		state == CommandBufferState::Initialized)
	{
		Begin();
	}
	// Can't be called during a render pass!
	Assert(state == CommandBufferState::Began);

	vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstLayout, regionCount, pRegions);
}

void VulkanCommandBuffer::CopyImage(VkImage srcImage, VkImageLayout srcLayout, VkImage dstImage, VkImageLayout dstLayout, u32 regionCount, const VkImageCopy* pRegions)
{
	if (state == CommandBufferState::Submitted ||
		state == CommandBufferState::Initialized)
	{
		Begin();
	}
	// Can't be called during a render pass!
	Assert(state == CommandBufferState::Began);

	vkCmdCopyImage(commandBuffer, srcImage, srcLayout, dstImage, dstLayout, regionCount, pRegions);
}

void VulkanCommandBuffer::SetViewport(u32 firstViewport, u32 viewportCount, const VkViewport * pViewports)
{
	vkCmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
}

void VulkanCommandBuffer::SetScissor(u32 firstScissor, u32 scissorCount, const VkRect2D * pScissors)
{
	vkCmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
}

void VulkanCommandBuffer::Dispatch(u32 workgroupX, u32 workgroupY, u32 workgroupZ)
{
	vkCmdDispatch(commandBuffer, workgroupX, workgroupY, workgroupZ);
}

void VulkanCommandBuffer::BindVertexBuffers(const VulkanVertexBuffer* const* vertexBuffers, const u32* vertexBufferOffsets, u32 bufferCount)
{
	if (bufferLevel == VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		Assert(IsInRenderPass());
	}

	DynamicArray<VkBuffer> nativeHandles(bufferCount);
	DynamicArray<VkDeviceSize> offsets(bufferCount);
	for (u32 i = 0; i < bufferCount; ++i)
	{
		nativeHandles[i] = vertexBuffers[i]->GetBuffer().handle;
		offsets[i] = vertexBufferOffsets[i];
	}

	vkCmdBindVertexBuffers(commandBuffer, 0, bufferCount, nativeHandles.GetData(), offsets.GetData());
}

void VulkanCommandBuffer::BindVertexBuffers(const VkBuffer* vertexBuffers, const u32* vertexBufferOffsets, u32 bufferCount)
{
	if (bufferLevel == VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		Assert(IsInRenderPass());
	}

	DynamicArray<VkDeviceSize> offsets(bufferCount);
	for (u32 i = 0; i < bufferCount; ++i)
	{
		offsets[i] = vertexBufferOffsets[i];
	}

	vkCmdBindVertexBuffers(commandBuffer, 0, bufferCount, vertexBuffers, offsets.GetData());
}

void VulkanCommandBuffer::BindIndexBuffer(const VulkanIndexBuffer& indexBuffer)
{
	if (bufferLevel == VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		Assert(IsInRenderPass());
	}
	vkCmdBindIndexBuffer(
		commandBuffer,
		indexBuffer.GetBuffer().handle,
		indexBuffer.GetBuffer().memory->alignedOffset,
		indexBuffer.GetIndexStride() == 4 ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16
	);
}

void VulkanCommandBuffer::BindIndexBuffer(VkBuffer indexBuffer, u32 bufferOffset, VkIndexType indexType)
{
	if (bufferLevel == VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		Assert(IsInRenderPass());
	}
	vkCmdBindIndexBuffer(
		commandBuffer,
		indexBuffer,
		bufferOffset,
		indexType
	);
}

void VulkanCommandBuffer::DrawIndexed(
	u32 indexCount, 
	u32 instanceCount, 
	u32 firstIndex, 
	i32 vertexOffset,
	u32 firstInstance
)
{
	vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanCommandBuffer::Draw(
	u32 vertexCount, 
	u32 instanceCount, 
	u32 firstVertex, 
	u32 firstInstance
)
{
	vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanCommandBuffer::ExecuteCommandBuffers(const DynamicArray<VulkanCommandBuffer*>& cmdBuffers)
{
	secondaryWithinCmdBuffer = cmdBuffers;
	DynamicArray<VkCommandBuffer> nativeBuffers(cmdBuffers.Size());
	for (u32 i = 0; i < cmdBuffers.Size(); ++i)
	{
		Assert(cmdBuffers[i]->GetLevel() == VK_COMMAND_BUFFER_LEVEL_SECONDARY);
		Assert(cmdBuffers[i]->HasEnded());
		nativeBuffers[i] = cmdBuffers[i]->GetNativeHandle();
	}

	vkCmdExecuteCommands(commandBuffer, nativeBuffers.Size(), nativeBuffers.GetData());
}

void VulkanCommandBuffer::Submit(
	const VulkanQueue& queue, 
	VkSemaphore waitSemaphore,
	VkPipelineStageFlags waitMask,
	VkSemaphore signalSemaphore
)
{
	Assert(state == CommandBufferState::PendingSubmit);

	// TODO - Figure out how to check that this submission can go through. Maybe...
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	if (waitSemaphore != VK_NULL_HANDLE)
	{
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitSemaphore;
		submitInfo.pWaitDstStageMask = &waitMask;
	}
	if (signalSemaphore != VK_NULL_HANDLE)
	{
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore;
	}

	VkResult result = vkQueueSubmit(queue.GetNativeHandle(), 1, &submitInfo, fence->GetNativeHandle());
	CHECK_VK(result);
	if (result != VK_SUCCESS)
	{

	}

	state = CommandBufferState::Submitted;
}

bool VulkanCommandBuffer::IsFreeForUse()
{
	FenceState fenceState = fence->CheckFenceStatus();
	bool freeForUse = fenceState == FenceState::Signaled;
	if (freeForUse)
	{
		for (auto& secondaryBuffer : secondaryWithinCmdBuffer)
		{
			secondaryBuffer->Reset(true);
		}
	}

	return freeForUse;
}

void VulkanCommandBuffer::Reset(bool forceReset)
{
	fence->ResetFence(forceReset);
	vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	state = CommandBufferState::Initialized;
	secondaryWithinCmdBuffer.Clear();
}

//////////////////////////////////////////////////////////////////////////

void VulkanCommandBufferManager::Initialize()
{
	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = logicalDevice.GetGraphicsQueue()->GetFamilyIndex();
	// TODO - Find out if there are any flags for creating command pools
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkResult result = vkCreateCommandPool(logicalDevice.GetNativeHandle(), &cmdPoolInfo, nullptr, &graphicsCmdPool);
	CHECK_VK(result);

	cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = logicalDevice.GetTransferQueue()->GetFamilyIndex();
	// TODO - Find out if there are any flags for creating command pools
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	result = vkCreateCommandPool(logicalDevice.GetNativeHandle(), &cmdPoolInfo, nullptr, &transferCmdPool);
	CHECK_VK(result);

}

void VulkanCommandBufferManager::SubmitGraphicsBuffer(
	bool waitForFence,
	VkSemaphore waitSemaphore,
	VkPipelineStageFlags waitMask,
	VkSemaphore signalSemaphore
)
{
	Assert(activeGraphicsBuffer);
	Assert(!activeTransferBuffer);

	SubmitCommandBuffer(*activeGraphicsBuffer, *logicalDevice.GetGraphicsQueue(), waitForFence, waitSemaphore, waitMask, signalSemaphore);

	activeGraphicsBuffer = nullptr;
}

void VulkanCommandBufferManager::SubmitTransferBuffer(
	bool waitForFence,
	VkSemaphore waitSemaphore,
	VkPipelineStageFlags waitMask,
	VkSemaphore signalSemaphore
)
{
	Assert(activeTransferBuffer);
	SubmitCommandBuffer(*activeTransferBuffer, *logicalDevice.GetTransferQueue(), waitForFence, waitSemaphore, waitMask, signalSemaphore);
	activeTransferBuffer = nullptr;
}

void VulkanCommandBufferManager::SubmitCommandBuffer(
	VulkanCommandBuffer& buffer,
	const VulkanQueue& queue,
	bool waitForFence,
	VkSemaphore waitSemaphore,
	VkPipelineStageFlags waitMask,
	VkSemaphore signalSemaphore
) const
{
	Assert(buffer.ContainsCommands());
	
	if (buffer.HasStarted())
	{
		if (buffer.IsInRenderPass())
		{
			buffer.EndRenderPass();
		}
		buffer.End();
	}

	buffer.Submit(queue, waitSemaphore, waitMask, signalSemaphore);
	if (waitForFence)
	{
		WaitFor(buffer);
	}
}

bool VulkanCommandBufferManager::WaitForGraphicsBuffer()
{
	return false;
}

bool VulkanCommandBufferManager::WaitForTransferBuffer()
{
	return false;
}

bool VulkanCommandBufferManager::HasValidGraphicsBuffer() const
{
	return activeGraphicsBuffer != nullptr;
}

bool VulkanCommandBufferManager::HasValidTransferBuffer() const
{
	return activeTransferBuffer != nullptr;
}

VulkanCommandBuffer* VulkanCommandBufferManager::CreateGraphicsCommandBuffer()
{
	Assert(graphicsCmdPool != VK_NULL_HANDLE);
	VulkanCommandBuffer* cBuffer = new VulkanCommandBuffer(logicalDevice, *this);
	cBuffer->Initialize(VK_COMMAND_BUFFER_LEVEL_PRIMARY, graphicsCmdPool);
	return cBuffer;
}

VulkanCommandBuffer* VulkanCommandBufferManager::CreateTransferCommandBuffer()
{
	Assert(transferCmdPool != VK_NULL_HANDLE);
	VulkanCommandBuffer* cBuffer = new VulkanCommandBuffer(logicalDevice, *this);
	cBuffer->Initialize(VK_COMMAND_BUFFER_LEVEL_PRIMARY, transferCmdPool);
	return cBuffer;
}

void VulkanCommandBufferManager::AllocateMissingCommandBuffers(u32 remainingCmdBufs, DynamicArray<VulkanCommandBuffer*>& outCommandBuffers)
{
	DynamicArray<VkCommandBuffer> buffers(remainingCmdBufs);

	VkCommandBufferAllocateInfo cmdAllocInfo = {};
	cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdAllocInfo.commandBufferCount = remainingCmdBufs;
	cmdAllocInfo.commandPool = graphicsCmdPool;
	cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	NOT_USED VkResult result = vkAllocateCommandBuffers(logicalDevice.GetNativeHandle(), &cmdAllocInfo, buffers.GetData());
	CHECK_VK(result);

	for (u32 i = 0; i < remainingCmdBufs; ++i)
	{
		VulkanCommandBuffer* cmdBuffer = new VulkanCommandBuffer(logicalDevice, *this);
		cmdBuffer->Initialize(buffers[i], VK_COMMAND_BUFFER_LEVEL_SECONDARY);
		graphicsSecondaryCommandBuffers.Add(cmdBuffer);
		outCommandBuffers.Add(cmdBuffer);
	}
}

VulkanCommandBuffer* VulkanCommandBufferManager::GetActiveGraphicsBuffer()
{
	if (activeTransferBuffer != nullptr)
	{
		SubmitTransferBuffer();
	}

	if (activeGraphicsBuffer == nullptr)
	{
		activeGraphicsBuffer = FindOrCreateGraphicsBuffer();
	}

	Assert(activeGraphicsBuffer);
	return activeGraphicsBuffer;
}

VulkanCommandBuffer* VulkanCommandBufferManager::GetActiveTransferBuffer()
{
	if (activeTransferBuffer == nullptr)
	{
		activeTransferBuffer = FindOrCreateTransferBuffer();
	}

	Assert(activeTransferBuffer);
	return activeTransferBuffer;
}

DynamicArray<VulkanCommandBuffer*> VulkanCommandBufferManager::GetSecondaryCommandBuffers(u32 numCmdBuffers)
{
	return FindOrCreateSecondaryBuffers(numCmdBuffers);
}

bool VulkanCommandBufferManager::WaitFor(VulkanCommandBuffer& buffer) const
{
	const u64 nanoSeconds = 10 * 1000000;
	buffer.GetFence()->WaitFor(nanoSeconds);
	return false;
}

VulkanCommandBuffer* VulkanCommandBufferManager::FindOrCreateGraphicsBuffer()
{
	VulkanCommandBuffer* found = nullptr;
	for (auto buffer : graphicsCommandBuffers)
	{
		if (buffer->IsSubmitted() &&
			buffer->IsFreeForUse())
		{
			found = buffer;
			found->Reset();
			break;
		}
	}

	if (!found)
	{
		found = CreateGraphicsCommandBuffer();
		graphicsCommandBuffers.Add(found);
		// TODO - Log debug info about adding new command buffer maybe?
	}

	return found;
}

VulkanCommandBuffer* VulkanCommandBufferManager::FindOrCreateTransferBuffer()
{
	VulkanCommandBuffer* found = nullptr;
	for (auto buffer : transferCommandBuffers)
	{
		if (buffer->IsFreeForUse())
		{
			found = buffer;
			found->Reset();
			break;
		}
	}

	if (!found)
	{
		found = CreateTransferCommandBuffer();
		transferCommandBuffers.Add(found);
		// TODO - Log debug info about adding new command buffer maybe?
	}

	return found;
}

DynamicArray<VulkanCommandBuffer*> VulkanCommandBufferManager::FindOrCreateSecondaryBuffers(u32 numCmdBuffers)
{
	DynamicArray<VulkanCommandBuffer*> commandBuffers;
	commandBuffers.Reserve(numCmdBuffers);

	for (auto buffer : graphicsSecondaryCommandBuffers)
	{
		// TODO - HACK: Need to figure out a way to treat secondary CBs a little differently...
		//if (buffer->IsFreeForUse())
		{
			buffer->Reset();
			commandBuffers.Add(buffer);
			if (commandBuffers.Size() == numCmdBuffers)
			{
				break;
			}
		}
	}

	if (commandBuffers.Size() < numCmdBuffers)
	{
		u32 cmdBuffersToAlloc = numCmdBuffers - commandBuffers.Size();
		AllocateMissingCommandBuffers(cmdBuffersToAlloc, commandBuffers);
	}

	return commandBuffers;
	
}

VulkanCommandBufferManager::VulkanCommandBufferManager(const VulkanDevice& device)
	: logicalDevice(device)
{
}

VulkanCommandBufferManager::~VulkanCommandBufferManager()
{
	vkDestroyCommandPool(logicalDevice.GetNativeHandle(), graphicsCmdPool, nullptr);
	vkDestroyCommandPool(logicalDevice.GetNativeHandle(), transferCmdPool, nullptr);
}
