#pragma once

#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/CommandBuffer.h"
#include "Apparition/Device.h"
#include "Apparition/ImageDescription.h"
#include "Apparition/Image.h"
#include "Apparition/Queue.h"
#include "BasicTypes/Intrinsics.hpp"

namespace Apparition
{
struct BackbufferSetupParams
{
    void* wndHandle = nullptr;
    u32 wndWidth, wndHeight = 0;
};

enum class BackbufferStatus
{
    Ready,
    Recreate,
    Unavailable
};

// NOTE: Will only set up rendering context once
APPARITION_API void SetupBackbuffer(Device device, const BackbufferSetupParams& params);
APPARITION_API void TeardownBackbuffer(Device device);

// Backing image acquisition
APPARITION_API BackbufferStatus AcquireBackbufferImage(Device device);
APPARITION_API void SubmitBackbufferCommandBuffer(CommandBuffer commandBuffer, Queue queue);
APPARITION_API void PresentBackbuffer(Queue presentQueue);

APPARITION_API ImageView GetBackBufferImageView(Device device);
APPARITION_API Image GetAcquiredBackbufferImage(Device device);

// Query backbuffer data
APPARITION_API u32 GetBackbufferWidth(Device device);
APPARITION_API u32 GetBackbufferHeight(Device device);
APPARITION_API ImageFormat::Type GetBackbufferFormat(Device device);
}
