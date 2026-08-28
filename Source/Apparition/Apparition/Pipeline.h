#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/Device.h"
#include "Apparition/DescriptorSet.h"
#include "Apparition/ImageDescription.h"
#include "Apparition/PipelineStateDefinitions.h"
#include "Apparition/ApparitionAPI.hpp"


struct AptnShaderData
{
    DynamicArray<u32> code;
    const char* entryName = nullptr;
};

struct AptnPipelineDescription
{
    DynamicArray<AptnDescriptorSetLayout> descriptorSets;
    bool useDescriptorHeaps = false;
};

HANDLE_TYPE(AptnVertexInputPipelineState);
HANDLE_TYPE(AptnPrerasterShadersPipelineState);
HANDLE_TYPE(AptnFragmentShaderPipelineState);
HANDLE_TYPE(AptnFragmentOutputPipelineState);
HANDLE_TYPE(AptnPipeline);

struct AptnVertexAttributeDescription
{
    u32 location = 0;
    u32 binding = 0;
    AptnVertexInputFormat format;
    u32 offset = 0;
};

struct AptnVertexBindingDescription
{
    u32 binding = 0;
    u32 stride = 0;
    AptnVertexInputRate inputRate;
};

struct AptnVertexInputPipelineStateCreationParams
{
    AptnPipelineDescription pipelineDesc;

    // Input Assembly
    AptnPrimitiveTopology primitiveTopology;

    // Vertex attributes
    DynamicArray<AptnVertexAttributeDescription> attributes;

    // Vertex bindings
    DynamicArray<AptnVertexBindingDescription> bindings;
};


struct AptnPreRasterShadersPipelineStateCreationParams
{
    AptnPipelineDescription pipelineDesc;

    // Rasterization State
    AptnFillMode fillMode = AptnFillMode::Full;
    AptnCullMode cullingMode = AptnCullMode::Back;
    AptnFrontFace frontFace = AptnFrontFace::CounterClockwise;
    f32 lineWidth = 1.f;

    // Vertex shader
    AptnShaderData vertexShader;
};


struct AptnFragmentShaderPipelineStateCreationParams
{
    AptnPipelineDescription pipelineDesc;

    // Fragment shader
    AptnShaderData fragmentShader;

    // Depth stencil state
    bool depthTestEnabled = false;
    bool depthWriteEnabled = false;
    AptnCompareOperation depthCompareOp = AptnCompareOperation::LessThanOrEqual;
    // TODO - support stencil
    
    // Multisample state
    // TODO - Support Multisampling
};


struct AptnColorBlendAttachment
{
    AptnBlendFactor srcColorFactor = AptnBlendFactor::One;
    AptnBlendFactor dstColorFactor = AptnBlendFactor::Zero;
    AptnBlendOperation colorBlendOperation = AptnBlendOperation::Add;
    AptnBlendFactor srcAlphaFactor = AptnBlendFactor::One;
    AptnBlendFactor dstAlphaFactor = AptnBlendFactor::Zero;
    AptnBlendOperation alphaBlendOperation = AptnBlendOperation::Add;
    AptnColorComponentFlags colorMask = AptnColorComponentFlags::RGBA;
};

struct AptnFragmentOutputPipelineStateCreationParams
{
    AptnPipelineDescription pipelineDesc;

    // Blend states
    DynamicArray<AptnColorBlendAttachment> attachments;

    // Multisample State
    //MultisampleState multisampleState;

    // Output
    DynamicArray<AptnImageFormat> colorAttachmentFormats;
    AptnImageFormat depthAttachmentFormat = AptnImageFormat::Invalid;
    AptnImageFormat stencilAttachmentFormat = AptnImageFormat::Invalid;
};


struct AptnPipelineCreationParams
{
    AptnPipelineDescription pipelineDesc;

    AptnVertexInputPipelineState vertexInput;
    AptnPrerasterShadersPipelineState prerasterShaders;
    AptnFragmentShaderPipelineState fragmentShader;
    AptnFragmentOutputPipelineState fragmentOutput;
};

namespace Apparition
{
NODISCARD APPARITION_API AptnVertexInputPipelineState CreateVertexInputPipelineState(AptnDevice device, const AptnVertexInputPipelineStateCreationParams& params);
APPARITION_API void DestroyVertexInputPipelineState(AptnVertexInputPipelineState state);
NODISCARD APPARITION_API AptnPrerasterShadersPipelineState CreatePrerasterShadersPipelineState(AptnDevice device, const AptnPreRasterShadersPipelineStateCreationParams& params);
APPARITION_API void DestroyPrerasterShadersPipelineState(AptnPrerasterShadersPipelineState state);
NODISCARD APPARITION_API AptnFragmentShaderPipelineState CreateFragmentShaderPipelineState(AptnDevice device, const AptnFragmentShaderPipelineStateCreationParams& params);
APPARITION_API void DestroyFragmentShaderPipelineState(AptnFragmentShaderPipelineState state);
NODISCARD APPARITION_API AptnFragmentOutputPipelineState CreateFragmentOutputPipelineState(AptnDevice device, const AptnFragmentOutputPipelineStateCreationParams& params);
APPARITION_API void DestroyFragmentOutputPipelineState(AptnFragmentOutputPipelineState state);

NODISCARD APPARITION_API AptnPipeline CreatePipeline(AptnDevice device, const AptnPipelineCreationParams& params);
APPARITION_API void DestroyPipeline(AptnPipeline pipeline);

}
