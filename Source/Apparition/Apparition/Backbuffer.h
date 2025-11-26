#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/Device.h"
#include "Apparition/ImageFormat.h"

namespace Apparition
{
struct BackbufferSetupParams
{
    void* wndHandle = nullptr;
    u32 wndWidth, wndHeight = 0;
};

// NOTE: Will only set up rendering context once
APPARITION_API void SetupBackbuffer(DeviceHandle device, const BackbufferSetupParams& params);
APPARITION_API void TeardownBackbuffer(DeviceHandle device);

// Query backbuffer data
APPARITION_API ImageFormat::Type GetBackbufferFormat(DeviceHandle device);

// TEMPORARY
APPARITION_API u32 GetBackbufferVkFormat(DeviceHandle device);
}
