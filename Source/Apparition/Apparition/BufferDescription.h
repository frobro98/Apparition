#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Utilities/EnumUtils.h"

enum class AptnBufferUsageFlags
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
ENUM_CLASS_OPERATORS(AptnBufferUsageFlags);

