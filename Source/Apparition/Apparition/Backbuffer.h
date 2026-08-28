#pragma once

#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/CommandBuffer.h"
#include "Apparition/Device.h"
#include "Apparition/ImageDescription.h"
#include "Apparition/Image.h"
#include "Apparition/Queue.h"
#include "BasicTypes/Intrinsics.hpp"

struct AptnBackbufferSetupParams
{
    void* wndHandle = nullptr;
    u32 wndWidth, wndHeight = 0;
};

enum class AptnBackbufferStatus
{
    Ready,
    Recreate,
    Unavailable
};

namespace Apparition
{
// NOTE: Will only set up rendering context once
APPARITION_API void SetupBackbuffer(AptnDevice device, const AptnBackbufferSetupParams& params);
APPARITION_API void TeardownBackbuffer(AptnDevice device);

// Backing image acquisition
APPARITION_API AptnBackbufferStatus AcquireBackbufferImage(AptnDevice device);
APPARITION_API void SubmitBackbufferCommandBuffer(AptnCommandBuffer commandBuffer, AptnQueue queue);
APPARITION_API void PresentBackbuffer(AptnQueue presentQueue);

APPARITION_API AptnImageView GetBackBufferImageView(AptnDevice device);
APPARITION_API AptnImage GetAcquiredBackbufferImage(AptnDevice device);

// Query backbuffer data
APPARITION_API u32 GetBackbufferWidth(AptnDevice device);
APPARITION_API u32 GetBackbufferHeight(AptnDevice device);
APPARITION_API AptnImageFormat GetBackbufferFormat(AptnDevice device);
}
