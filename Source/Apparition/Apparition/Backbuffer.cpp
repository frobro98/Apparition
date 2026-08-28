
#include "Backbuffer.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/HandleDefinitions.h"
#include "Internal/ImageFormatConversion.h"
#include "Internal/VulkanInfos.h"

namespace Apparition
{
void SetupBackbuffer(AptnDevice device, const AptnBackbufferSetupParams& params)
{
    Assert(apparition.deviceManager);

    return apparition.deviceManager->SetupBackbuffer(device, params);
}

void TeardownBackbuffer(AptnDevice device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.TeardownBackbuffer(device);
}

AptnBackbufferStatus AcquireBackbufferImage(AptnDevice device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.AcquireNextBackbufferImage(device);
}

void SubmitBackbufferCommandBuffer(AptnCommandBuffer commandBuffer, AptnQueue queue)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    const u32 deviceIndex = GetDeviceIndexFromHandle(commandBuffer);
    const DeviceInternal& deviceInternal = deviceManager.DeviceInternalFrom(deviceIndex);
    const u32 handleIndex = GetHandleIndex(commandBuffer);
    const CommandBufferInternal& cbInternal = GetCommandBufferInternalFromIndex(deviceInternal, handleIndex);
    Assert(!cbInternal.hasBegun);

    QueueInternal& queueInternal = GetQueueInternal(queue);

    const Backbuffer& backbuffer = deviceInternal.backbuffer;
    NOT_USED const u32 imageIndex = backbuffer.currentImageIndex;
    //*
    VkSemaphore waitSemaphores[] = { backbuffer.isImageAvailableSem };
    VkSemaphore signalSemaphores[] = { backbuffer.hasRenderingFinishedSem };
    //*/
    /*
    VkSemaphore waitSemaphores[] = { backbuffer.acquireImageSemaphores[imageIndex] };
    VkSemaphore signalSemaphores[] = { backbuffer.submitRenderSemaphores[imageIndex] };
    //*/
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo;
    Vk::ZeroInfoStruct(submitInfo, VK_STRUCTURE_TYPE_SUBMIT_INFO);

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cbInternal.commandBuffer;
    VkResult result = vkQueueSubmit(queueInternal.queue, 1, &submitInfo, VK_NULL_HANDLE);
    CHECK_VK(result);
}

void PresentBackbuffer(AptnQueue presentQueue)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    const u32 deviceIndex = GetDeviceIndexFromHandle(presentQueue);
    const DeviceInternal& deviceInternal = deviceManager.DeviceInternalFrom(deviceIndex);

    QueueInternal& queueInternal = GetQueueInternal(presentQueue);
    Assert(queueInternal.canPresent);

    const Backbuffer& backbuffer = deviceInternal.backbuffer;
    NOT_USED const u32 imageIndex = backbuffer.currentImageIndex;

    VkPresentInfoKHR presentInfo;
    Vk::ZeroInfoStruct(presentInfo, VK_STRUCTURE_TYPE_PRESENT_INFO_KHR);
    presentInfo.waitSemaphoreCount = 1;
    //*
    presentInfo.pWaitSemaphores = &backbuffer.hasRenderingFinishedSem;
    //*/
    /*
    presentInfo.pWaitSemaphores = &backbuffer.submitRenderSemaphores[imageIndex];
    //*/
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &backbuffer.swapchainHandle;
    presentInfo.pImageIndices = &backbuffer.currentImageIndex;
    presentInfo.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(queueInternal.queue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        //Recreate()
    }
    else if (result != VK_SUCCESS)
    {
        // TODO - Log
        Assert(false);
    }

    // TODO: DO NOT DO THIS!
    result = vkQueueWaitIdle(queueInternal.queue);
    CHECK_VK(result);
}

AptnImageView GetBackBufferImageView(AptnDevice device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.GetBackbufferImageView(device);
}

AptnImage GetAcquiredBackbufferImage(AptnDevice device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.GetAcquiredBackbufferImage(device);
}

u32 GetBackbufferWidth(AptnDevice device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    const DeviceInternal& deviceInternals = deviceManager.DeviceInternalFrom(device);
    return deviceInternals.backbuffer.extents.width;
}
u32 GetBackbufferHeight(AptnDevice device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    const DeviceInternal& deviceInternals = deviceManager.DeviceInternalFrom(device);
    return deviceInternals.backbuffer.extents.height;
}

AptnImageFormat GetBackbufferFormat(AptnDevice device)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    const DeviceInternal& deviceInternals = deviceManager.DeviceInternalFrom(device);
    return deviceInternals.backbuffer.format;
}
}