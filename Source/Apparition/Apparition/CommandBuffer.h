#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/Device.h"
#include "Apparition/ApparitionAPI.hpp"

struct VkCommandBuffer_T;
typedef struct VkCommandBuffer_T* VkCommandBuffer;

namespace Apparition
{
struct CommandPool
{
    u64 handle;
};
HANDLE_TYPE_OPERATORS(CommandPool);

struct CommandPoolCreationParams
{
    u32 queueIndex = 0;
    bool canResetCommandBuffers = false;
};

NODISCARD APPARITION_API CommandPool CreateCommandPool(Device deviceHandle, const CommandPoolCreationParams& params);
APPARITION_API void DestroyCommandPool(CommandPool commandPoolHandle);

struct CommandBuffer
{
    u64 handle;
};
HANDLE_TYPE_OPERATORS(CommandBuffer);

struct CommandBufferAllocParams
{
    // Sets if the command buffer is a primary buffer or if another command buffer will handle submission
    bool isSecondary = false;
};

NODISCARD APPARITION_API CommandBuffer AllocateCommandBuffer(CommandPool commandPoolHandle, const CommandBufferAllocParams& params = CommandBufferAllocParams());
APPARITION_API void FreeCommandBuffer(CommandBuffer commandBufferHandle);
APPARITION_API void ResetCommandBuffer(CommandBuffer commandBuffer);

// TEMPORARILY HERE
APPARITION_API VkCommandBuffer GetVulkanHandle(CommandBuffer commandBufferHandle);
}
