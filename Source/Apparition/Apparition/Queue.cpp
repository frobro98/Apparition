
#include "Queue.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/HandleDefinitions.h"
#include "Internal/VulkanInfos.h"

namespace Apparition
{
Queue AllocateGraphicsQueue(Device device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.AllocateGraphicsQueue(device);
}

Queue AllocateTransferQueue(Device device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.AllocateTransferQueue(device);
}

// TODO: Support compute and graphics queues being the same queue
//Queue AllocateComputeQueue(Device device)
//{
//    Assert(apparition.deviceManager);
//    DeviceManager& deviceManager = *apparition.deviceManager;
//    return deviceManager.AllocateComputeQueue(device);
//}

void FreeQueue(Queue queue)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.FreeQueue(queue);
}
APPARITION_API void SubmitCommandBuffer(Queue queue, CommandBuffer commandBuffer)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    const u32 deviceIndex = GetDeviceIndexFromHandle(queue);
    DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
    // Queue internals
    const u32 queueIndex = GetHandleIndex(queue);
    const DynamicArray<QueueInternal> queueArray = deviceManager.GetQueueArray(deviceInternal, GetResourcePoolIndexFromHandle(queue));
    QueueInternal queueInternal = queueArray[queueIndex - 1];

    // Command Buffer internals
    const u32 commandBufferIndex = GetHandleIndex(commandBuffer);
    const CommandBufferInternal& cbInternal = deviceInternal.commandBuffers[commandBufferIndex - 1];
    Assert(!cbInternal.hasBegun);

    VkSubmitInfo submitInfo = {};
    Vk::ZeroInfoStruct(submitInfo, VK_STRUCTURE_TYPE_SUBMIT_INFO);
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cbInternal.commandBuffer;
    VkResult result = vkQueueSubmit(queueInternal.queue, 1, &submitInfo, VK_NULL_HANDLE);
    CHECK_VK(result);
}
APPARITION_API void WaitForIdle(Queue queue)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    const u32 deviceIndex = GetDeviceIndexFromHandle(queue);
    DeviceInternal& deviceInternal = deviceManager.GetDeviceInternals(deviceIndex);
    // Queue internals
    const u32 queueIndex = GetHandleIndex(queue);
    const DynamicArray<QueueInternal> queueArray = deviceManager.GetQueueArray(deviceInternal, GetResourcePoolIndexFromHandle(queue));
    QueueInternal queueInternal = queueArray[queueIndex - 1];

    vkQueueWaitIdle(queueInternal.queue);
}
}
