#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/Device.h"
#include "Apparition/DescriptorSet.h"
#include "Apparition/ImageDescription.h"
#include "Apparition/PipelineStateDefinitions.h"
#include "Apparition/ApparitionAPI.hpp"

namespace Apparition
{

struct ShaderData
{
    DynamicArray<u32> code;
    const char* entryName = nullptr;
};

struct PipelineDescription
{
    DynamicArray<DescriptorSetLayout> DescriptorSets;
};

HANDLE_TYPE(VertexInputPipelineState);
HANDLE_TYPE(PrerasterShadersPipelineState);
HANDLE_TYPE(FragmentShaderPipelineState);
HANDLE_TYPE(FragmentOutputPipelineState);
HANDLE_TYPE(Pipeline);

struct VertexAttributeDescription
{
    u32 location = 0;
    u32 binding = 0;
    VertexInputFormat::Type format;
    u32 offset = 0;
};

struct VertexBindingDescription
{
    u32 binding = 0;
    u32 stride = 0;
    VertexInputRate::Type inputRate;
};

struct VertexInputPipelineStateCreationParams
{
    // Input Assembly
    PrimitiveTopology::Type primitiveTopology;

    // Vertex attributes
    DynamicArray<VertexAttributeDescription> attributes;

    // Vertex bindings
    DynamicArray<VertexBindingDescription> bindings;
};

NODISCARD APPARITION_API VertexInputPipelineState CreateVertexInputPipelineState(Device device, const VertexInputPipelineStateCreationParams& params);
APPARITION_API void DestroyVertexInputPipelineState(VertexInputPipelineState state);

struct PreRasterShadersPipelineStateCreationParams
{
    PipelineDescription pipelineDesc;

    // Rasterization State
    FillMode::Type fillMode = FillMode::Full;
    CullMode::Type cullingMode = CullMode::Back;
    FrontFace::Type frontFace = FrontFace::CounterClockwise;
    f32 lineWidth = 1.f;

    // Vertex shader
    ShaderData vertexShader;
};

NODISCARD APPARITION_API PrerasterShadersPipelineState CreatePrerasterShadersPipelineState(Device device, const PreRasterShadersPipelineStateCreationParams& params);
APPARITION_API void DestroyPrerasterShadersPipelineState(PrerasterShadersPipelineState state);

struct FragmentShaderPipelineStateCreationParams
{
    PipelineDescription pipelineDesc;

    // Fragment shader
    ShaderData fragmentShader;

    // Depth stencil state
    bool depthTestEnabled = false;
    bool depthWriteEnabled = false;
    CompareOperation::Type depthCompareOp = CompareOperation::LessThanOrEqual;
    // TODO - support stencil
    
    // Multisample state
    // TODO - Support Multisampling
};

NODISCARD APPARITION_API FragmentShaderPipelineState CreateFragmentShaderPipelineState(Device device, const FragmentShaderPipelineStateCreationParams& params);
APPARITION_API void DestroyFragmentShaderPipelineState(FragmentShaderPipelineState state);

struct ColorBlendAttachment
{
    BlendFactor::Type srcColorFactor = BlendFactor::One;
    BlendFactor::Type dstColorFactor = BlendFactor::Zero;
    BlendOperation::Type colorBlendOperation = BlendOperation::Add;
    BlendFactor::Type srcAlphaFactor = BlendFactor::One;
    BlendFactor::Type dstAlphaFactor = BlendFactor::Zero;
    BlendOperation::Type alphaBlendOperation = BlendOperation::Add;
    ColorComponentFlags colorMask = ColorComponentFlagBits::RGBA;
};

struct FragmentOutputPipelineStateCreationParams
{
    // Blend states
    DynamicArray<ColorBlendAttachment> attachments;

    // Multisample State
    //MultisampleState multisampleState;

    // Output
    DynamicArray<ImageFormat::Type> colorAttachmentFormats;
    ImageFormat::Type depthAttachmentFormat = ImageFormat::Invalid;
    ImageFormat::Type stencilAttachmentFormat = ImageFormat::Invalid;
};

NODISCARD APPARITION_API FragmentOutputPipelineState CreateFragmentOutputPipelineState(Device device, const FragmentOutputPipelineStateCreationParams& params);
APPARITION_API void DestroyFragmentOutputPipelineState(FragmentOutputPipelineState state);

struct PipelineCreationParams
{
    PipelineDescription pipelineDesc;

    VertexInputPipelineState vertexInput;
    PrerasterShadersPipelineState prerasterShaders;
    FragmentShaderPipelineState fragmentShader;
    FragmentOutputPipelineState fragmentOutput;
};

NODISCARD APPARITION_API Pipeline CreatePipeline(Device device, const PipelineCreationParams& params);
APPARITION_API void DestroyPipeline(Pipeline pipeline);

}
