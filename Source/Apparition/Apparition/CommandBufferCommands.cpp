#include "CommandBufferCommands.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/HandleDefinitions.h"
#include "Internal/VulkanInfos.h"

namespace Apparition
{
APPARITION_API void BeginCommandBuffer(CommandBuffer commandBuffer, bool oneTimeSubmit)
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

APPARITION_API void EndCommandBuffer(CommandBuffer commandBuffer)
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

APPARITION_API void CopyBuffer(CommandBuffer commandBuffer, const BufferCopyDesc& copyDesc)
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
}
