#pragma once

#include "Apparition/BufferDescription.h"
#include "Apparition/Device.h"
#include "BasicTypes/Intrinsics.hpp"

namespace Apparition
{
struct Buffer
{
    u64 handle;
};

// Currently supports simple memory handling internally.
// 
// Unless there's a greater need for control over specifics when allocating, 
// like Dedicated allocation or more specialized allocation flags, this will
// stay how the implementation functions
//
// NOTE: Buffers do not **currently** support concurrent sharing modes. Each buffer 
// is exclusive. Will address this if and when this becomes a problem
struct BufferCreationParams
{
    BufferUsageFlags usage;
    size_t size = 0;
    bool supportsMappedMemory = false;
};

NODISCARD APPARITION_API Buffer CreateBuffer(Device device, const BufferCreationParams& params);
APPARITION_API void DestroyBuffer(Buffer buffer);

// Buffer Map/Unmap
NODISCARD APPARITION_API void* MapBuffer(Buffer buffer);
APPARITION_API void UnmapBuffer(Buffer buffer);
}