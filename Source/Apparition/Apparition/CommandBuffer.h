#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/Device.h"
#include "Apparition/ApparitionAPI.hpp"

struct VkCommandBuffer_T;
typedef struct VkCommandBuffer_T* VkCommandBuffer;

HANDLE_TYPE(AptnCommandPool);

struct AptnCommandPoolCreationParams
{
    u32 queueIndex = 0;
    bool canResetCommandBuffers = false;
};

HANDLE_TYPE(AptnCommandBuffer);

struct AptnCommandBufferAllocParams
{
    // Sets if the command buffer is a primary buffer or if another command buffer will handle submission
    bool isSecondary = false;
};

namespace Apparition
{
NODISCARD APPARITION_API AptnCommandPool CreateCommandPool(AptnDevice deviceHandle, const AptnCommandPoolCreationParams& params);
APPARITION_API void DestroyCommandPool(AptnCommandPool commandPoolHandle);

NODISCARD APPARITION_API AptnCommandBuffer AllocateCommandBuffer(AptnCommandPool commandPoolHandle, const AptnCommandBufferAllocParams& params = AptnCommandBufferAllocParams());
APPARITION_API void FreeCommandBuffer(AptnCommandBuffer commandBufferHandle);
APPARITION_API void ResetCommandBuffer(AptnCommandBuffer commandBuffer);
}
