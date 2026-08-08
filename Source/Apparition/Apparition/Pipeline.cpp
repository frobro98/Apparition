
#include "Apparition/Pipeline.h"

#include "Apparition/Internal/ApparitionInternals.h"

namespace Apparition
{
AptnVertexInputPipelineState CreateVertexInputPipelineState(AptnDevice device, const AptnVertexInputPipelineStateCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateVertexInputPipelineState(device, params);
}

void DestroyVertexInputPipelineState(AptnVertexInputPipelineState state)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyVertexInputPipelineState(state);
}

AptnPrerasterShadersPipelineState CreatePrerasterShadersPipelineState(AptnDevice device, const AptnPreRasterShadersPipelineStateCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreatePrerasterShadersPipelineState(device, params);
}

void DestroyPrerasterShadersPipelineState(AptnPrerasterShadersPipelineState state)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyPrerasterShadersPipelineState(state);
}

AptnFragmentShaderPipelineState CreateFragmentShaderPipelineState(AptnDevice device, const AptnFragmentShaderPipelineStateCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateFragmentShaderPipelineState(device, params);
}

void DestroyFragmentShaderPipelineState(AptnFragmentShaderPipelineState state)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyFragmentShaderPipelineState(state);
}

AptnFragmentOutputPipelineState CreateFragmentOutputPipelineState(AptnDevice device, const AptnFragmentOutputPipelineStateCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateFragmentOutputPipelineState(device, params);
}

void DestroyFragmentOutputPipelineState(AptnFragmentOutputPipelineState state)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyFragmentOutputPipelineState(state);
}

AptnPipeline CreatePipeline(AptnDevice device, const AptnPipelineCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreatePipeline(device, params);
}

void DestroyPipeline(AptnPipeline pipeline)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyPipeline(pipeline);
}

}
