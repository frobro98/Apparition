#pragma once

#include "BasicTypes/Intrinsics.hpp"


namespace AptnBufferUsageFlagBits
{
enum Type
{
    TransferSrc = 1 << 0,
    TransferDst = 1 << 1,
    UniformBuffer = 1 << 2,
    StorageBuffer = 1 << 3,
    VertexBuffer = 1 << 4,
    IndexBuffer = 1 << 5,
    ShaderDeviceAddress = 1 << 6,

    MAX = 0x7FFFFFFF
};
} // BufferUsageFlags
using AptnBufferUsageFlags = u32;


