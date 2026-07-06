
#include "Apparition/Pipeline.h"

#include "Apparition/Internal/ApparitionInternals.h"

namespace Apparition
{
VertexInputPipelineState CreateVertexInputPipelineState(Device device, const VertexInputPipelineStateCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateVertexInputPipelineState(device, params);
}

void DestroyVertexInputPipelineState(VertexInputPipelineState state)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyVertexInputPipelineState(state);
}

PrerasterShadersPipelineState CreatePrerasterShadersPipelineState(Device device, const PreRasterShadersPipelineStateCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreatePrerasterShadersPipelineState(device, params);
}

void DestroyPrerasterShadersPipelineState(PrerasterShadersPipelineState state)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyPrerasterShadersPipelineState(state);
}

FragmentShaderPipelineState CreateFragmentShaderPipelineState(Device device, const FragmentShaderPipelineStateCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateFragmentShaderPipelineState(device, params);
}

void DestroyFragmentShaderPipelineState(FragmentShaderPipelineState state)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyFragmentShaderPipelineState(state);
}

FragmentOutputPipelineState CreateFragmentOutputPipelineState(Device device, const FragmentOutputPipelineStateCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateFragmentOutputPipelineState(device, params);
}

void DestroyFragmentOutputPipelineState(FragmentOutputPipelineState state)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyFragmentOutputPipelineState(state);
}

Pipeline CreatePipeline(Device device, const PipelineCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreatePipeline(device, params);
}

void DestroyPipeline(Pipeline pipeline)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyPipeline(pipeline);
}

}
