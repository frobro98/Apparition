#pragma once

#include "Apparition/CommandBuffer.h"
#include "ApparitionInternals.h"
#include "VulkanDefinitions.h"

using namespace Apparition;

class CommandBufferManager
{
public:
    // Allows a command pool to be used outside of the Device. Management of the command pools still lives with the Device
    void RegisterCommandPool(const CommandPoolInternals& commandPool);
    // Removes a command pool, preventing outside usage from the Device. Mostly done when destroying the internal VkCommandPool
    void UnregisterCommandPool(const CommandPoolInternals& commandPool);

    CommandBufferHandle AllocateCommandBuffer(CommandPoolHandle commandPoolHandle, const CommandBufferAllocParams& params);
    void FreeCommandBuffer(CommandPoolHandle commandPoolHandle, CommandBufferHandle commandBufferHandle);
    void ResetCommandBuffer(CommandBufferHandle commandBufferHandle);

private:
    CommandPoolInternals& FindCommandPool(CommandPoolHandle commandPoolHandle);
    CommandBufferInternals& FindCommandBuffer(CommandBufferHandle commandBufferHandle);

private:
    // NOTE(nblane): consider some sort of mailbox system for handles, which would allow 
    // Allows access to all of the registered command pools to be iterated on to find the corresponding VkCommandPool
    DynamicArray<CommandPoolInternals> commandPools;
    // Allows iteration on all of the allocated CommandBuffer objects to find the corresponding VkCommandBuffer
    DynamicArray<CommandBufferInternals> commandBuffers;

    static inline u32 NextCommandBufferHandle = 1;
};