#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/Device.h"

namespace Apparition
{
struct SwapchainHandle
{
    u64 Handle;
};

struct BackbufferSetupParams
{
    void* wndHandle = nullptr;
    u32 wndWidth, wndHeight = 0;
};

// NOTE: Will only set up rendering context once
APPARITION_API void SetupBackbuffer(DeviceHandle device, const BackbufferSetupParams& params);
APPARITION_API void TeardownBackbuffer(DeviceHandle device);
}
