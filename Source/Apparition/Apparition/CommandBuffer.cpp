
#include "CommandBuffer.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/VulkanDefinitions.h"

namespace Apparition
{

CommandPoolHandle CreateCommandPool(DeviceHandle deviceHandle, const CommandPoolCreationParams& params)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.CreateCommandPool(deviceHandle, params);
}

void DestroyCommandPool(CommandPoolHandle commandPoolHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.DestroyCommandPool(commandPoolHandle);
}


CommandBufferHandle AllocateCommandBuffer(CommandPoolHandle commandPoolHandle, const CommandBufferAllocParams& params)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.AllocateCommandBuffer(commandPoolHandle, params);
}

void FreeCommandBuffer(CommandBufferHandle commandBufferHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.FreeCommandBuffer(commandBufferHandle);
}

VkCommandBuffer GetVulkanHandle(CommandBufferHandle commandBufferHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.GetCommandBufferHandle(commandBufferHandle);
}

}