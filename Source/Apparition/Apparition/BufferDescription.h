#pragma once

#include "BasicTypes/Intrinsics.hpp"

namespace Apparition
{
namespace BufferUsageFlagBits
{
enum Type
{
    TransferSrc = 1 << 0,
    TransferDst = 1 << 1,
    UniformBuffer = 1 << 2,
    StorageBuffer = 1 << 3,
    VertexBuffer = 1 << 4,
    IndexBuffer = 1 << 5,

    MAX = 0x7FFFFFFF
};
} // BufferUsageFlags
using BufferUsageFlags = u32;
} // Apparition


