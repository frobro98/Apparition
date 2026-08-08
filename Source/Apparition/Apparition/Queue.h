#pragma once

#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/CommandBuffer.h"
#include "Apparition/Device.h"
#include "BasicTypes/Intrinsics.hpp"

HANDLE_TYPE(AptnQueue);

namespace Apparition
{
// NOTE: There is no use currently for supporting multiple queues within a queue family
APPARITION_API AptnQueue AllocateGraphicsQueue(AptnDevice device);
APPARITION_API AptnQueue AllocateTransferQueue(AptnDevice device);
//APPARITION_API Queue AllocateComputeQueue(Device device);
APPARITION_API void FreeQueue(AptnQueue queue);

APPARITION_API void SubmitCommandBuffer(AptnQueue queue, AptnCommandBuffer commandBuffer);
APPARITION_API void WaitForIdle(AptnQueue queue);

APPARITION_API u32 GetQueueIndex(AptnQueue queue);
}
