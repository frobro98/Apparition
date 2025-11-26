
#include "CommandBufferManager.h"

#include "VulkanInfos.h"

void CommandBufferManager::RegisterCommandPool(const CommandPoolInternals& commandPool)
{
    // NOTE: there isn't any way of knowing if this is a duplicate or not. May not be a problem, since 
    // it's controlled by the DeviceManager, but still could pose a problem
    commandPools.Add(commandPool);
}

void CommandBufferManager::UnregisterCommandPool(const CommandPoolInternals& commandPool)
{
    for (u32 i = 0; i < commandPools.Size(); ++i)
    {
        if (commandPools[i].handle.handle == commandPool.handle.handle)
        {
            commandPools.Remove(i);
            break;
        }
    }
}

CommandBufferHandle CommandBufferManager::AllocateCommandBuffer(CommandPoolHandle commandPoolHandle, const CommandBufferAllocParams& params)
{
    CommandPoolInternals& poolInternals = FindCommandPool(commandPoolHandle);

    VkCommandBufferAllocateInfo allocInfo;
    Vk::ZeroInfoStruct(allocInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO);
    allocInfo.commandPool = poolInternals.cmdPool;
    allocInfo.level = !params.isSecondary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocInfo.commandBufferCount = 1;

    CommandBufferInternals cbInternals = {};
    cbInternals.owningDevice = poolInternals.owningDevice;
    VkResult result = vkAllocateCommandBuffers(poolInternals.owningDevice, nullptr, &cbInternals.commandBuffer);
    CHECK_VK(result);


    CommandBufferHandle cbHandle{
        .handle = NextCommandBufferHandle++
    };
    cbInternals.handle = cbHandle;

    // Store the newly allocated command buffer within the CommandPool as well as in the manager's array
    commandBuffers.Add(cbInternals);
    poolInternals.allocatedCommandBuffers.Add(cbInternals);

    return cbHandle;
}

void CommandBufferManager::FreeCommandBuffer(CommandPoolHandle commandPoolHandle, CommandBufferHandle commandBufferHandle)
{
    CommandPoolInternals& poolInternals = FindCommandPool(commandPoolHandle);

    // Remove internals from pool
    u32 index = poolInternals.allocatedCommandBuffers.Size();
    for (u32 i = 0; i < poolInternals.allocatedCommandBuffers.Size(); ++i)
    {
        if (poolInternals.allocatedCommandBuffers[i].handle.handle == commandBufferHandle.handle)
        {
            index = i;
            break;
        }
    }
    Assert(poolInternals.allocatedCommandBuffers.IsIndexValid(index));
    poolInternals.allocatedCommandBuffers.Remove(index);

    // Remove internals from manager
    index = commandBuffers.Size();
    for (u32 i = 0; i < commandBuffers.Size(); ++i)
    {
        if (commandBuffers[i].handle.handle == commandBufferHandle.handle)
        {
            index = i;
            break;
        }
    }
    Assert(commandBuffers.IsIndexValid(index));
    CommandBufferInternals cbInternals = commandBuffers[index];
    commandBuffers.Remove(index);

    vkFreeCommandBuffers(cbInternals.owningDevice, poolInternals.cmdPool, 1, &cbInternals.commandBuffer);
}

void CommandBufferManager::ResetCommandBuffer(CommandBufferHandle commandBufferHandle)
{
    CommandBufferInternals& cbInternals = FindCommandBuffer(commandBufferHandle);
    VkResult result = vkResetCommandBuffer(cbInternals.commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    CHECK_VK(result);
}

CommandPoolInternals& CommandBufferManager::FindCommandPool(CommandPoolHandle commandPoolHandle)
{
    CommandPoolInternals* poolInternals = nullptr;
    for (u32 i = 0; i < commandPools.Size(); ++i)
    {
        if (commandPools[i].handle.handle == commandPoolHandle.handle)
        {
            poolInternals = &commandPools[i];
            break;
        }
    }

    Assert(poolInternals);
    return *poolInternals;
}

CommandBufferInternals& CommandBufferManager::FindCommandBuffer(CommandBufferHandle commandBufferHandle)
{
    CommandBufferInternals* cbInternals = nullptr;
    for (u32 i = 0; i < commandPools.Size(); ++i)
    {
        if (commandBuffers[i].handle.handle == commandBufferHandle.handle)
        {
            cbInternals = &commandBuffers[i];
            break;
        }
    }

    Assert(cbInternals);
    return *cbInternals;
}
