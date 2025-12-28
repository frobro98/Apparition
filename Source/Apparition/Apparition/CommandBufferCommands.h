#pragma once

#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/Buffer.h"
#include "Apparition/CommandBuffer.h"
#include "Apparition/Image.h"
#include "Apparition/ImageDescription.h"
#include "Apparition/RenderingDescription.h"

// TODO: Revisit when implementing specific Sascha Williams functionality
namespace Apparition
{
APPARITION_API void BeginCommandBuffer(CommandBuffer commandBuffer, bool oneTimeSubmit = false);
APPARITION_API void EndCommandBuffer(CommandBuffer commandBuffer);

APPARITION_API void BeginRendering(CommandBuffer commandBuffer, const RenderSetupParams& renderSetupParams);
APPARITION_API void EndRendering(CommandBuffer commandBuffer);

struct BindVertexBufferDesc
{
    Buffer vertexBuffer;
};

struct BindIndexBufferDesc
{
    Buffer indexBuffer;
};

APPARITION_API void BindVertexBuffers(CommandBuffer commandBuffer, const BindVertexBufferDesc& desc);
APPARITION_API void BindIndexBuffer(CommandBuffer commandBuffer, const BindIndexBufferDesc& desc);

struct ViewportDesc
{
    f32 x = 0.f;
    f32 y = 0.f;
    f32 width = 0.f;
    f32 height = 0.f;
};

struct ScissorDesc
{
    i32 offsetX = 0;
    i32 offsetY = 0;
    u32 extentX = 0;
    u32 extentY = 0;
};

APPARITION_API void SetViewportAndScissor(CommandBuffer commandBuffer, const ViewportDesc& viewDesc, const ScissorDesc& scissorDesc);

// Draw Commands
APPARITION_API void DrawIndexed(CommandBuffer commandBuffer, u32 indexCount);

// Copy Commands

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

/* NOTES ON PIPELINE BARRIERS
*  What we know:
*   - vkCmdPipelineBarriers2 takes a list of multiple different barriers
*   - There are various access points within the pipeline that we can have these barriers 
*    start/stop at, via VkPipelineStageFlags2 and VkAccessFlags2
*   - The goal of Apparition is to expose vulkan in a simplified but purposeful way
*     - This goal could be that we want to allow the users to have control over the access 
*      and stage flags for a pipeline barrier
*   - If we wanted to expose access and stage flags, we would need to only expose ones that we
*    care about and grow from there. After all, this is a personal project, not a one size fits all
*   - The main pieces of data we need to expose is AccessMasks and StageMasks
*   - Currently, there's only ImageMemoryBarrier and MemoryBarrier that I care about. BufferMemoryBarrier
*    has unclear usage, so I'm not worried about it
*/

struct ImageMemoryBarrierDesc
{
    Image image;
    ImageAccess::Type access;
    Apparition::ImageViewAspect::Type aspect;
};

// NOTE: current does not support multiple image transitions at one time, BUT will need to support this at some point
APPARITION_API void ImageMemoryBarrier(CommandBuffer commandBuffer, const ImageMemoryBarrierDesc& barrierDesc);
}
