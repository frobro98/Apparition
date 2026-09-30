#pragma once

#include "Apparition/Internal/DeviceManager.h"

REGISTER_HANDLE_TYPE(Queue, queues);

REGISTER_HANDLE_TYPE(CommandPool, commandPools);
REGISTER_HANDLE_TYPE(CommandBuffer, commandBuffers);
// Resource Handles
REGISTER_HANDLE_TYPE(Buffer, bufferResources);
REGISTER_HANDLE_TYPE(Image, imageResources);
REGISTER_HANDLE_TYPE(ImageView, imageViewResources);
REGISTER_HANDLE_TYPE(Sampler, samplerResources);
// Pipeline Handles
REGISTER_HANDLE_TYPE(VertexInputPipelineState, vertexInputResources);
REGISTER_HANDLE_TYPE(PrerasterShadersPipelineState, prerasterShadersResources);
REGISTER_HANDLE_TYPE(FragmentShaderPipelineState, fragmentShaderResources);
REGISTER_HANDLE_TYPE(FragmentOutputPipelineState, fragmentOutputResources);
REGISTER_HANDLE_TYPE(Pipeline, pipelineResources);
// DescriptorHeap
REGISTER_HANDLE_TYPE(SamplerHeap, samplerHeaps);
REGISTER_HANDLE_TYPE(ResourceHeap, resourceHeaps);
// DescriptorSet Handles
REGISTER_HANDLE_TYPE(DescriptorSetLayout, descriptorSetLayouts);
REGISTER_HANDLE_TYPE(DescriptorPool, descriptorPools);
REGISTER_HANDLE_TYPE(DescriptorSet, descriptorSets);

