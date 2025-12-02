#pragma once

#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/Buffer.h"
#include "Apparition/CommandBuffer.h"

namespace Apparition
{
APPARITION_API void BeginCommandBuffer(CommandBuffer commandBuffer, bool oneTimeSubmit = false);
APPARITION_API void EndCommandBuffer(CommandBuffer commandBuffer);

struct BufferCopyDesc
{
    size_t size = 0;
    Buffer srcBuffer = { InvalidHandle };
    Buffer dstBuffer = { InvalidHandle };
    // Optional
    size_t srcOffset = 0;
    size_t dstOffset = 0;
};

APPARITION_API void CopyBuffer(CommandBuffer commandBuffer, const BufferCopyDesc& copyDesc);
}
