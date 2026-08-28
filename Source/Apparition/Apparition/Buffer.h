#pragma once

#include "Apparition/BufferDescription.h"
#include "Apparition/Device.h"
#include "BasicTypes/Intrinsics.hpp"

struct VkBuffer_T;
typedef struct VkBuffer_T* VkBuffer;

// Currently supports simple memory handling internally.
// 
// Unless there's a greater need for control over specifics when allocating, 
// like Dedicated allocation or more specialized allocation flags, this will
// stay how the implementation functions
//
// NOTE: Buffers do not **currently** support concurrent sharing modes. Each buffer 
// is exclusive. Will address this if and when this becomes a problem
struct AptnBufferCreationParams
{
    AptnBufferUsageFlags usage;
    size_t size = 0;
    bool supportsMappedMemory = false;
};

HANDLE_TYPE(AptnBuffer)
// TODO - Think about if this needs to be a handle or just a value
using AptnBufferAddress = u64;

namespace Apparition
{
NODISCARD APPARITION_API AptnBuffer CreateBuffer(AptnDevice device, const AptnBufferCreationParams& params);
APPARITION_API void DestroyBuffer(AptnBuffer buffer);

// Buffer Map/Unmap
NODISCARD APPARITION_API void* MapBuffer(AptnBuffer buffer);
APPARITION_API void UnmapBuffer(AptnBuffer buffer);

// Buffer Device Address Access
NODISCARD APPARITION_API AptnBufferAddress GetBufferDeviceAddress(AptnBuffer /*buffer*/);
}