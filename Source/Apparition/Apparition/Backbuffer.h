#pragma once

#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/Device.h"
#include "Apparition/ImageDescription.h"
#include "BasicTypes/Intrinsics.hpp"

namespace Apparition
{
struct BackbufferSetupParams
{
    void* wndHandle = nullptr;
    u32 wndWidth, wndHeight = 0;
};

// NOTE: Will only set up rendering context once
APPARITION_API void SetupBackbuffer(Device device, const BackbufferSetupParams& params);
APPARITION_API void TeardownBackbuffer(Device device);

// Query backbuffer data
APPARITION_API ImageFormat::Type GetBackbufferFormat(Device device);

// TEMPORARY
APPARITION_API u32 GetBackbufferVkFormat(Device device);
}
