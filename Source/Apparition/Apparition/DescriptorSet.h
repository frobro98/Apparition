#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/StaticArray.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/Buffer.h"
#include "Apparition/Device.h"
#include "Apparition/Image.h"
#include "Apparition/ApparitionAPI.hpp"

struct VkSampler_T;
typedef struct VkSampler_T* VkSampler;

namespace AptnDescriptor
{
enum Type
{
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

    Count,
    Max = 0x7FFFFFFF
};
}
static_assert(AptnDescriptor::Count == AptnDescriptor::InputAttachment + 1);

namespace AptnShaderStageFlagBits
{
enum Type
{
    Vertex = 1 << 0,
    Fragment = 1 << 1,

    Max = 0x7FFFFFFF
};
}
using AptnShaderStageFlags = u32;

HANDLE_TYPE(AptnDescriptorSetLayout);
HANDLE_TYPE(AptnDescriptorPool);
HANDLE_TYPE(AptnDescriptorSet);

namespace AptnDescriptorFlagBits
{
enum Type
{
    UpdateAfter = 1 << 0,
    UpdateUnusedWhilePending = 1 << 1,
    PartiallyBound = 1 << 2,
    VariableDescriptorCount = 1 << 3,

    Max = 0x7FFFFFFF
};
}
using AptnDescriptorFlags = u32;

// TODO - Specify specific flags, related to descriptor_indexing
struct AptnDescriptorSetLayoutDesc
{
    u32 binding = 0;
    AptnDescriptor::Type descriptorType = AptnDescriptor::Type::Max;
    u32 descriptorCount = 0;
    AptnShaderStageFlags shaderStageFlags = 0;
    AptnDescriptorFlags flags = 0;
};
struct AptnDescriptorSetLayoutCreationParams
{
    DynamicArray<AptnDescriptorSetLayoutDesc> bindings;
};

struct AptnDescriptorPoolSize
{
    AptnDescriptor::Type poolType = AptnDescriptor::Max;
    u32 size = 0;
};

struct AptnDescriptorPoolCreationParams
{
    DynamicArray<AptnDescriptorPoolSize> poolSizes;
};

struct AptnDescriptorSetAllocParams
{
    AptnDescriptorSetLayout layout;
};

HANDLE_TYPE(AptnSampler);

namespace AptnSamplerAddressMode
{
enum Type
{
    Repeat,
    Clamp,
    Mirror
};
}

namespace AptnSamplerFilter
{
enum Type
{
    Nearest,
    Linear
};
}

namespace AptnSamplerMipmapMode
{
enum Type
{
    Nearest,
    Linear
};
}

struct AptnSamplerCreationParams
{
    AptnSamplerFilter::Type filter = AptnSamplerFilter::Linear;
    AptnSamplerAddressMode::Type addressModeU = AptnSamplerAddressMode::Clamp;
    AptnSamplerAddressMode::Type addressModeV = AptnSamplerAddressMode::Clamp;
    AptnSamplerMipmapMode::Type mipMode = AptnSamplerMipmapMode::Linear;
    f32 maxAnisotropy = 0;
    f32 minLod = 0;
    f32 maxLod = 1;
};


// Update Descriptor Set functionality
struct AptnBufferDescriptorInfo
{
    AptnBuffer buffer = { AptnInvalidHandle };
    u64 offset = 0;
    u64 range = 0;
};

struct AptnImageDescriptorInfo
{
    AptnSampler sampler = { AptnInvalidHandle };
    AptnImageView imageView = { AptnInvalidHandle };
    AptnImageAccess::Type access = AptnImageAccess::Undefined;
};

struct AptnUpdateDescriptorSetDesc
{
    AptnDescriptorSet descriptorSet = { AptnInvalidHandle };
    u32 setBinding = 0;
    AptnDescriptor::Type descriptorType = AptnDescriptor::Max;
    AptnImageDescriptorInfo* imageDescriptor = nullptr;
    AptnBufferDescriptorInfo* bufferDescriptor = nullptr;
};

namespace Apparition
{
NODISCARD APPARITION_API AptnDescriptorSetLayout CreateDescriptorSetLayout(AptnDevice device, const AptnDescriptorSetLayoutCreationParams& params);
APPARITION_API void DestroyDescriptorSetLayout(AptnDescriptorSetLayout descriptorSetLayout);

NODISCARD APPARITION_API AptnDescriptorPool CreateDescriptorPool(AptnDevice device, const AptnDescriptorPoolCreationParams& params);
APPARITION_API void DestroyDescriptorPool(AptnDescriptorPool descriptorPool);

NODISCARD APPARITION_API AptnDescriptorSet AllocateDescriptorSet(AptnDescriptorPool descriptorPool, const AptnDescriptorSetAllocParams& allocParams);
APPARITION_API void AllocateDescriptorSets(AptnDescriptorPool descriptorPool, const DynamicArray<AptnDescriptorSetAllocParams>& allocParams);
APPARITION_API void FreeDescriptorSet(AptnDescriptorSet descriptorSet);
APPARITION_API void FreeDescriptorSets(const DynamicArray<AptnDescriptorSet> descriptorSets);

NODISCARD APPARITION_API AptnSampler CreateSampler(AptnDevice device, const AptnSamplerCreationParams& params);
APPARITION_API void DestroySampler(AptnSampler sampler);

APPARITION_API void UpdateDescriptorSets(const DynamicArray<AptnUpdateDescriptorSetDesc>& descriptorSetUpdates);
}