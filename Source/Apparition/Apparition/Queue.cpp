
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
void SubmitCommandBuffer(Queue queue, CommandBuffer commandBuffer)
{
    QueueInternal& queueInternal = GetQueueInternal(queue);
    const CommandBufferInternal& cbInternal = GetCommandBufferInternal(commandBuffer);
    Assert(!cbInternal.hasBegun);
    Assert(cbInternal.queueFamilyIndex == queueInternal.queueFamilyIndex);

    VkSubmitInfo submitInfo = {};
    Vk::ZeroInfoStruct(submitInfo, VK_STRUCTURE_TYPE_SUBMIT_INFO);
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cbInternal.commandBuffer;
    VkResult result = vkQueueSubmit(queueInternal.queue, 1, &submitInfo, VK_NULL_HANDLE);
    CHECK_VK(result);
}
void WaitForIdle(Queue queue)
{
    QueueInternal& queueInternal = GetQueueInternal(queue);

    vkQueueWaitIdle(queueInternal.queue);
}

u32 GetQueueIndex(Queue queue)
{
    return GetQueueInternal(queue).queueFamilyIndex;
}
}
