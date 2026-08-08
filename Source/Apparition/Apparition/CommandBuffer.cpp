
#include "CommandBuffer.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/VulkanDefinitions.h"

namespace Apparition
{
AptnCommandPool CreateCommandPool(AptnDevice deviceHandle, const AptnCommandPoolCreationParams& params)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.CreateCommandPool(deviceHandle, params);
}

void DestroyCommandPool(AptnCommandPool commandPoolHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.DestroyCommandPool(commandPoolHandle);
}

AptnCommandBuffer AllocateCommandBuffer(AptnCommandPool commandPoolHandle, const AptnCommandBufferAllocParams& params)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.AllocateCommandBuffer(commandPoolHandle, params);
}

void FreeCommandBuffer(AptnCommandBuffer commandBufferHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.FreeCommandBuffer(commandBufferHandle);
}

void ResetCommandBuffer(AptnCommandBuffer commandBuffer)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.ResetCommandBuffer(commandBuffer);
}
}