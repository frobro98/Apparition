#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Apparition/Buffer.h"
#include "Apparition/Device.h"
#include "Apparition/Image.h"

HANDLE_TYPE(AptnSamplerHeap)
HANDLE_TYPE(AptnResourceHeap)

struct AptnSamplerHeapCreationParams
{
    u64 samplerCount = 0;
};

struct AptnResourceHeapCreationParams
{
    u64 bufferCount = 0;
    u64 imageCount = 0;
};

enum class AptnDescriptor
{
    None,
    Sampler,
    CombinedImageSampler,
    SampledImage,
    StorageImage,
    UniformTexelBuffer,
    StorageTexelBuffer,
    UniformBuffer,
    StorageBuffer,
    UniformBufferDynamic,
    StorageBufferDynamic,
    InputAttachment,
};

enum class AptnSamplerAddressMode
{
    Repeat,
    Clamp,
    Mirror
};

enum class AptnSamplerFilter
{
    Nearest,
    Linear
};

enum class AptnSamplerMipmapMode
{
    Nearest,
    Linear
};

struct AptnSamplerDescriptor
{
    AptnSamplerFilter filter = AptnSamplerFilter::Linear;
    AptnSamplerAddressMode addressModeU = AptnSamplerAddressMode::Clamp;
    AptnSamplerAddressMode addressModeV = AptnSamplerAddressMode::Clamp;
    AptnSamplerMipmapMode mipMode = AptnSamplerMipmapMode::Linear;
    f32 maxAnisotropy = 0;
    f32 minLod = 0;
    f32 maxLod = 1;
};

struct AptnBufferAddressDescriptor
{
    AptnBufferAddress address = 0;
    u64 size = 0;
    AptnDescriptor type = AptnDescriptor::None;
};

struct AptnImageDescriptor
{
    AptnImage image = { AptnInvalidHandle };
    AptnDescriptor type = AptnDescriptor::None;
    AptnImageAccess access = AptnImageAccess::Undefined;
    AptnImageFormat format = AptnImageFormat::Invalid;
    AptnImageAspectFlags aspect = AptnImageAspectFlags::Color;
    u32 mipCount = 1;
    u32 baseMipLevel = 0;
};

namespace Apparition
{
APPARITION_API AptnSamplerHeap CreateSamplerHeap(AptnDevice device, const AptnSamplerHeapCreationParams& params);
APPARITION_API void DestroySamplerHeap(AptnSamplerHeap samplerHeap);

APPARITION_API u64 QuerySamplerHeapCapacity(AptnSamplerHeap samplerHeap);
APPARITION_API u64 QuerySamplerHeapDescriptorCount(AptnSamplerHeap samplerHeap);

APPARITION_API void WriteSamplerDescriptor(AptnSamplerHeap samplerHeap, const AptnSamplerDescriptor& samplerDescriptor);
APPARITION_API void WriteSamplerDescriptors(AptnSamplerHeap samplerHeap, u64 samplerDescriptorCount, const AptnSamplerDescriptor* samplerDescriptors);
APPARITION_API void CommitSamplerDescriptors(AptnSamplerHeap samplerHeap);

APPARITION_API AptnResourceHeap CreateResourceHeap(AptnDevice device, const AptnResourceHeapCreationParams& params);
APPARITION_API void DestroyResourceHeap(AptnResourceHeap resourceHeap);

APPARITION_API u64 QueryResourceHeapCapacity(AptnResourceHeap resourceHeap);
APPARITION_API u64 QueryResourceHeapDescriptorCount(AptnResourceHeap resourceHeap);

APPARITION_API void WriteBufferAddressDescriptor(AptnResourceHeap resourceHeap, const AptnBufferAddressDescriptor& bufferAddr);
APPARITION_API void WriteImageResourceDescriptor(AptnResourceHeap resourceHeap, const AptnImageDescriptor& imageDescriptor);
APPARITION_API void WriteBufferAddressDescriptors(AptnResourceHeap resourceHeap, const AptnBufferAddressDescriptor* bufferAddrDescriptors, u64 descriptorCount);
APPARITION_API void WriteImageResourceDescriptor(AptnResourceHeap resourceHeap, const AptnImageDescriptor* imageDescriptors, u64 descriptorCount);
APPARITION_API void CommitResourceDescriptors(AptnResourceHeap resourceHeap);
}
