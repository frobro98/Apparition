
#include "CommandBuffer.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/VulkanDefinitions.h"

namespace Apparition
{

CommandPool CreateCommandPool(Device deviceHandle, const CommandPoolCreationParams& params)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.CreateCommandPool(deviceHandle, params);
}

void DestroyCommandPool(CommandPool commandPoolHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.DestroyCommandPool(commandPoolHandle);
}


CommandBuffer AllocateCommandBuffer(CommandPool commandPoolHandle, const CommandBufferAllocParams& params)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.AllocateCommandBuffer(commandPoolHandle, params);
}

void FreeCommandBuffer(CommandBuffer commandBufferHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.FreeCommandBuffer(commandBufferHandle);
}

void ResetCommandBuffer(CommandBuffer commandBuffer)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.ResetCommandBuffer(commandBuffer);
}

VkCommandBuffer GetVulkanHandle(CommandBuffer commandBufferHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.GetCommandBufferHandle(commandBufferHandle);
}
}