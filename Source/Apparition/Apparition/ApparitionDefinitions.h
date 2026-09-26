#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Utilities/EnumUtils.h"

// Apparition Allocation Flags

enum class AptnAllocationScope
{
    Command, // Command allocation
    Object,  // Specific object allocation
    Cache,   // PipelineCache allocation
    Device,  // Device allocation
    Instance // Instance allocation
};

enum class AptnValidationSeverity
{
    None,
    Verbose,
    Info,
    Warning,
    Error
};

enum class AptnDebugMessageType
{
    General = 0x00000001,
    Validation = 0x00000002,
    Performance = 0x00000004,
};

enum class AptnBufferUsageFlags
{
    TransferSrc = 1 << 0,
    TransferDst = 1 << 1,
    UniformBuffer = 1 << 2,
    StorageBuffer = 1 << 3,
    VertexBuffer = 1 << 4,
    IndexBuffer = 1 << 5,
    IndirectBuffer = 1 << 6,
    ShaderDeviceAddress = 1 << 7
};
ENUM_CLASS_OPERATORS(AptnBufferUsageFlags);

enum class AptnImageFormat
{
    RGB_8norm,
    RGB_8u,
    RGB_16f,
    BGR_8norm,
    RGBA_8norm,
    RGBA_8u,
    RGBA_16f,
    BGRA_8norm,
    Gray_8norm,
    BC1,
    BC3,
    BC7,
    DS_32f_8u,
    DS_24f_8u,
    D_32f,
    Invalid,

    Count
};

enum class AptnImageAspectFlags
{
    Color = 1 << 0,
    Depth = 1 << 1,
    Stencil = 1 << 2,
    DepthStencil = Depth | Stencil
};
ENUM_CLASS_OPERATORS(AptnImageAspectFlags);

enum class AptnImageAccess
{
    Undefined,
    Present,
    TransferSrc,
    TransferDst,
    ColorWrite,
    ColorRead,
    DepthStencilWrite,
    DepthStencilRead
};

enum class AptnImageUsageFlags
{
    TransferSrc = 1 << 0,
    TransferDst = 1 << 1,
    Sampled = 1 << 2,
    ColorAttachment = 1 << 3,
    DepthStencilAttachment = 1 << 4
};
ENUM_CLASS_OPERATORS(AptnImageUsageFlags);

////////////////////////
// Vertex Input State
////////////////////////
enum class AptnVertexInputFormat
{
    F32_1,
    F32_2,
    F32_3,
    F32_4,
    U8_4,
    U32,
    I32,

    MAX = 0x7FFFFFFF
};

enum class AptnVertexInputRate
{
    Vertex,
    Instance,

    MAX = 0x7FFFFFFF
};

////////////////////////
// Input Assembly State
////////////////////////

enum class AptnPrimitiveTopology
{
    TriangleList,
    TriangleStrip,
    TriangleFan,
    LineList,
    LineStrip,
    PointList,

    MAX = 0x7FFFFFFF
};


////////////////////////
// Rasterization State
////////////////////////

enum class AptnFillMode
{
    Full,
    Wireframe,
    Point,
    MAX = 0x7FFFFFFF
};

enum class AptnCullMode
{
    None,
    Back,
    Front,
    FrontAndBack,

    MAX = 0x7FFFFFFF
};

enum class AptnFrontFace
{
    Clockwise,
    CounterClockwise,

    MAX = 0x7FFFFFFF
};

////////////////////////
// Depth/Stencil State
////////////////////////

enum class AptnCompareOperation
{
    None,
    Equal,
    NotEqual,
    Less,
    LessThanOrEqual,
    Greater,
    GreaterThanOrEqual,
    Always
};

enum class AptnStencilOperation
{
    Keep,
    Zero,
    Replace,
    Invert,
    IncrementAndClamp,
    DecrementAndClamp,
    IncrementAndWrap,
    DecrementAndWrap
};

////////////////////////
// Multisampling State
////////////////////////

enum class AptnSampleCountFlags
{
    SampleCount_1 = 1 << 0,
    SampleCount_2 = 1 << 1,
    SampleCount_4 = 1 << 2,
    SampleCount_8 = 1 << 3,
    SampleCount_16 = 1 << 4,
    SampleCount_32 = 1 << 5,
    SampleCount_64 = 1 << 6,

    MAX = 0x7FFFFFFF
};
ENUM_CLASS_OPERATORS(AptnSampleCountFlags);

////////////////////////
// Color Blend State
////////////////////////

enum class AptnBlendMode
{
    Opaque,
    Transparent,

    MAX = 0x7FFFFFFF
};

enum class AptnColorComponentFlags
{
    Red = 1 << 0,
    Green = 1 << 1,
    Blue = 1 << 2,
    Alpha = 1 << 3,

    RGB = Red | Green | Blue,
    RGBA = Red | Green | Blue | Alpha
};
ENUM_CLASS_OPERATORS(AptnColorComponentFlags);

enum class AptnBlendOperation
{
    None,
    Add,
    Subtract
};

enum class AptnBlendFactor
{
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    ConstColor,
    OneMinusConstColor,
    ConstAlpha,
    OneMinusConstAlpha
};

enum class AptnLoadOperation : u8
{
    Load,
    Clear,
    DontCare,

    Count = DontCare + 1
};
static_assert((__underlying_type(AptnLoadOperation))AptnLoadOperation::Count < 4, "LoadOperation must be less that 3 bit");

enum class AptnStoreOperation : u8
{
    Store,
    DontCare,

    Count = DontCare + 1
};
static_assert((__underlying_type(AptnStoreOperation))AptnStoreOperation::Count < 4, "StoreOperation must be less that 3 bit");

#define ATTACHMENT_OP_MASK 2
#define CREATE_ATTACHMENT_OP(LoadOp, StoreOp) (((u8)AptnLoadOperation::LoadOp << ATTACHMENT_OP_MASK) | ((u8)AptnStoreOperation::StoreOp))
enum class AptnAttachmentOperations
{
    DontLoad_DontStore = CREATE_ATTACHMENT_OP(DontCare, DontCare),
    DontLoad_Store = CREATE_ATTACHMENT_OP(DontCare, Store),
    Load_DontStore = CREATE_ATTACHMENT_OP(Load, DontCare),
    Load_Store = CREATE_ATTACHMENT_OP(Load, Store),
    Clear_DontStore = CREATE_ATTACHMENT_OP(Clear, DontCare),
    Clear_Store = CREATE_ATTACHMENT_OP(Clear, Store)
};

constexpr AptnLoadOperation LoadOperationFrom(AptnAttachmentOperations op)
{
    return (AptnLoadOperation)((__underlying_type(AptnAttachmentOperations))op >> ATTACHMENT_OP_MASK);
}

constexpr AptnStoreOperation StoreOperationFrom(AptnAttachmentOperations op)
{
    return (AptnStoreOperation)((__underlying_type(AptnAttachmentOperations))op & ((1 << ATTACHMENT_OP_MASK) - 1));
}

constexpr AptnAttachmentOperations AttachmentOperationsFrom(AptnLoadOperation loadOp, AptnStoreOperation storeOp)
{
    return (AptnAttachmentOperations)(((__underlying_type(AptnLoadOperation))loadOp << ATTACHMENT_OP_MASK) | ((__underlying_type(AptnStoreOperation))storeOp));
}

#undef ATTACHMENT_OP_MASK
#undef CREATE_ATTACHMENT_OP

