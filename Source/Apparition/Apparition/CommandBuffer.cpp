
#include "CommandBuffer.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/CommandBufferManager.h"

namespace Apparition
{

CommandPoolHandle CreateCommandPool(DeviceHandle deviceHandle, const CommandPoolCreationParams& params)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	return deviceManager.CreateCommandPool(deviceHandle, params);
}

void DestroyCommandPool(DeviceHandle deviceHandle, CommandPoolHandle commandPoolHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	deviceManager.DestroyCommandPool(deviceHandle, commandPoolHandle);
}


CommandBufferHandle AllocateCommandBuffer(CommandPoolHandle commandPoolHandle, const CommandBufferAllocParams& params)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	CommandBufferManager& cmdBufferManager = deviceManager.GetCommandBufferManager();
	return cmdBufferManager.AllocateCommandBuffer(commandPoolHandle, params);
}

void FreeCommandBuffer(CommandPoolHandle commandPoolHandle, CommandBufferHandle commandBufferHandle)
{
	Assert(apparition.deviceManager);
	DeviceManager& deviceManager = *apparition.deviceManager;
	CommandBufferManager& cmdBufferManager = deviceManager.GetCommandBufferManager();
	return cmdBufferManager.FreeCommandBuffer(commandPoolHandle, commandBufferHandle);
}



}