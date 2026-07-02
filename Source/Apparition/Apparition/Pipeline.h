#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/Device.h"
#include "Apparition/ApparitionAPI.hpp"

namespace Apparition
{
struct PipelineDescription
{

};

struct VertexInput
{
    u64 handle;
};
HANDLE_TYPE_OPERATORS(VertexInput);

struct VertexInputPipelinePartCreateInfo
{
    PipelineDescription PipelineDesc;

    // Input Assembly

    // Vertex attributes

    // Vertex bindings
};

struct PreRasterShaders
{
    u64 handle;
};
HANDLE_TYPE_OPERATORS(PreRasterShaders);

struct PreRasterShadersPipelinePartCreateInfo
{
    PipelineDescription PipelineDesc;

    // Vertex shader

};

struct FragmentShader
{
    u64 handle;
};
HANDLE_TYPE_OPERATORS(FragmentShader);

struct FragmentShaderPipelinePartCreateInfo
{
    PipelineDescription PipelineDesc;

    // Fragment shader

    // Depth stencil state

    // Multisample state
};

struct FragmentOutput
{
    u64 handle;
};
HANDLE_TYPE_OPERATORS(FragmentOutput);

struct FragmentOutputPipelinePartCreateInfo
{
    PipelineDescription PipelineDesc;

    // Blend states
};

struct PipelineCreateInfo
{
    PipelineDescription PipelineDesc;
};

// PipelineLayouts can be destroyed after they're used with Maintenance4
// Curious if creating the pipeline once using the pipeline vs creating the 
// 4 parts and linking them makes sense?
// 
// The 4 parts are more to manage, but offer important optimization benefits
// and can be reused. However, a pipeline layout will need to be created and 
// destroyed every time 3 of them are created, AND THE PIPELINE IS LINKED.
// Not terribly important since they can create the layout struct and pass that in
// 
// One pipeline is one thing to manage, but will need a lot more data within the 
// creation struct
struct PipelineCreateInfo
{
    // Shaders

    // Dynamic state is set, so nothing here

    // Vertex Information
    //  - Input State
    //  - Binding Description

    // Input Assembly

    // Viewport State
};
}
