#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Apparition/Device.h"

#include "Apparition/ApparitionAPI.hpp"

struct VkCommandBuffer_T;
typedef struct VkCommandBuffer_T* VkCommandBuffer;

namespace Apparition
{
struct CommandPoolHandle
{
    u64 handle;
};

struct CommandPoolCreationParams
{
    u32 queueIndex = 0;
};

NODISCARD APPARITION_API CommandPoolHandle CreateCommandPool(DeviceHandle deviceHandle, const CommandPoolCreationParams& params);
APPARITION_API void DestroyCommandPool(DeviceHandle deviceHandle, CommandPoolHandle commandPoolHandle);

struct CommandBufferHandle
{
    u64 handle;
};

struct CommandBufferAllocParams
{
    // Sets if the command buffer is a primary buffer or if another command buffer will handle submission
    bool isSecondary = false;
};

NODISCARD APPARITION_API CommandBufferHandle AllocateCommandBuffer(CommandPoolHandle commandPoolHandle, const CommandBufferAllocParams& params);
APPARITION_API void FreeCommandBuffer(CommandBufferHandle commandBufferHandle);

// TEMPORARILY HERE
APPARITION_API VkCommandBuffer GetVulkanHandle(CommandBufferHandle commandBufferHandle);
}
