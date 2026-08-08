
#include "Queue.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/HandleDefinitions.h"
#include "Internal/VulkanInfos.h"

namespace Apparition
{
AptnQueue AllocateGraphicsQueue(AptnDevice device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.AllocateGraphicsQueue(device);
}

AptnQueue AllocateTransferQueue(AptnDevice device)
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

void FreeQueue(AptnQueue queue)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.FreeQueue(queue);
}
void SubmitCommandBuffer(AptnQueue queue, AptnCommandBuffer commandBuffer)
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
void WaitForIdle(AptnQueue queue)
{
    QueueInternal& queueInternal = GetQueueInternal(queue);

    vkQueueWaitIdle(queueInternal.queue);
}

u32 GetQueueIndex(AptnQueue queue)
{
    return GetQueueInternal(queue).queueFamilyIndex;
}
}
