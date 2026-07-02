#pragma once

#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/CommandBuffer.h"
#include "Apparition/Device.h"
#include "BasicTypes/Intrinsics.hpp"

namespace Apparition
{
struct Queue
{
    u64 handle;
};
HANDLE_TYPE_OPERATORS(Queue);

// NOTE: There is no use currently for supporting multiple queues within a queue family
APPARITION_API Queue AllocateGraphicsQueue(Device device);
APPARITION_API Queue AllocateTransferQueue(Device device);
//APPARITION_API Queue AllocateComputeQueue(Device device);
APPARITION_API void FreeQueue(Queue queue);

APPARITION_API void SubmitCommandBuffer(Queue queue, CommandBuffer commandBuffer);
APPARITION_API void WaitForIdle(Queue queue);
}
