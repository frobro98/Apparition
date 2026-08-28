#pragma once

#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/Buffer.h"
#include "Apparition/CommandBuffer.h"
#include "Apparition/Image.h"
#include "Apparition/ImageDescription.h"
#include "Apparition/Pipeline.h"
#include "Apparition/RenderingDescription.h"

// TODO: Revisit when implementing specific Sascha Williams functionality
struct AptnBindVertexBufferDesc
{
    AptnBuffer vertexBuffer;
};

struct AptnBindIndexBufferDesc
{
    AptnBuffer indexBuffer;
};

struct AptnViewportDesc
{
    f32 x = 0.f;
    f32 y = 0.f;
    f32 width = 0.f;
    f32 height = 0.f;
};

struct AptnScissorDesc
{
    i32 offsetX = 0;
    i32 offsetY = 0;
    u32 extentX = 0;
    u32 extentY = 0;
};

enum class AptnBindPoint
{
    Graphics,
    Compute
};

struct AptnBindDescriptorSetsDesc
{
    AptnPipelineDescription pipelineDesc;
    AptnBindPoint bindPoint = AptnBindPoint::Graphics;
    u32 firstSet = 0;
    DynamicArray<AptnDescriptorSet> descriptorSets;
};

struct AptnPushDataDesc
{
    void* dataAddress = nullptr;
    u64 dataSize = 0;
};

struct AptnBufferCopyDesc
{
    size_t size = 0;
    AptnBuffer srcBuffer = { AptnInvalidHandle };
    AptnBuffer dstBuffer = { AptnInvalidHandle };
    // Optional
    size_t srcOffset = 0;
    size_t dstOffset = 0;
};

struct AptnBufferToImageCopyOutline
{
    u64 bufferOffset = 0;
    // Subresource
    AptnImageAspectFlags aspect = AptnImageAspectFlags::Color;
    u32 mipLevel = 0;
    u32 imgWidth = 0;
    u32 imgHeight = 0;
    // TODO - Support array layers
};

struct AptnBufferToImageCopyDesc
{
    AptnBuffer srcBuffer = { AptnInvalidHandle };
    AptnImage dstImage = { AptnInvalidHandle };
    AptnBufferToImageCopyOutline outline;
};

struct AptnBufferRegionsToImageCopyDesc
{
    AptnBuffer srcBuffer = { AptnInvalidHandle };
    AptnImage dstImage = { AptnInvalidHandle };
    DynamicArray<AptnBufferToImageCopyOutline> outlines;
};

struct AptnBlitImageDesc
{
    AptnImage srcImage;
    AptnImageAspectFlags srcAspect = AptnImageAspectFlags::Color;
    u32 srcMipLevel = 0;
    // TODO - Better understand why there are multiple offsets for both src and dst
    i32 srcOffsetX[2] = { 0, 0 };
    i32 srcOffsetY[2] = { 0, 0 };
    AptnImage dstImage;
    AptnImageAspectFlags dstAspect = AptnImageAspectFlags::Color;
    u32 dstMipLevel = 0;
    i32 dstOffsetX[2] = { 0, 0 };
    i32 dstOffsetY[2] = { 0, 0 };
};

struct AptnImageMemoryBarrierDesc
{
    AptnImage image;
    AptnImageAccess access = AptnImageAccess::Undefined;
    // Subresource range
    AptnImageAspectFlags aspect = AptnImageAspectFlags::Color;
    u32 baseMipLevel = 0;
    u32 mipLevelCount = 0;
};

namespace Apparition
{
APPARITION_API void BeginCommandBuffer(AptnCommandBuffer commandBuffer, bool oneTimeSubmit = false);
APPARITION_API void EndCommandBuffer(AptnCommandBuffer commandBuffer);

APPARITION_API void BeginRendering(AptnCommandBuffer commandBuffer, const AptnRenderSetupParams& renderSetupParams);
APPARITION_API void EndRendering(AptnCommandBuffer commandBuffer);

APPARITION_API void BindVertexBuffers(AptnCommandBuffer commandBuffer, const AptnBindVertexBufferDesc& desc);
APPARITION_API void BindIndexBuffer(AptnCommandBuffer commandBuffer, const AptnBindIndexBufferDesc& desc);

APPARITION_API void SetViewportAndScissor(AptnCommandBuffer commandBuffer, const AptnViewportDesc& viewDesc, const AptnScissorDesc& scissorDesc);

APPARITION_API void BindGraphicsPipeline(AptnCommandBuffer commandBuffer, AptnPipeline pipeline);

APPARITION_API void BindDescriptorSets(AptnCommandBuffer commandBuffer, const AptnBindDescriptorSetsDesc& bindDescriptorSetsDesc);

APPARITION_API void BindSamplerHeap(AptnCommandBuffer commandBuffer, AptnSamplerHeap samplerHeap);
APPARITION_API void BindResourceHeap(AptnCommandBuffer commandBuffer, AptnResourceHeap resourceHeap);

APPARITION_API void PushData(AptnCommandBuffer commandBuffer, const AptnPushDataDesc& pushData);

// Draw Commands
APPARITION_API void DrawIndexed(AptnCommandBuffer commandBuffer, u32 indexCount, u32 firstIndex, u32 instanceCount = 1, u32 firstInstance = 0);

// Copy Commands
APPARITION_API void CopyBuffer(AptnCommandBuffer commandBuffer, const AptnBufferCopyDesc& copyDesc);
APPARITION_API void CopyBufferToImage(AptnCommandBuffer commandBuffer, const AptnBufferToImageCopyDesc& copyDesc);
APPARITION_API void CopyBufferRegionsToImage(AptnCommandBuffer commandBuffer, const AptnBufferRegionsToImageCopyDesc& copyRegions);


APPARITION_API void BlitImage(AptnCommandBuffer commandBuffer, const AptnBlitImageDesc& blitDesc);

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

// NOTE: current does not support multiple image transitions at one time, BUT will need to support this at some point
APPARITION_API void ImageMemoryBarrier(AptnCommandBuffer commandBuffer, const AptnImageMemoryBarrierDesc& barrierDesc);
}
