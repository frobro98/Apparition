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

namespace Apparition
{
namespace Descriptor
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
static_assert(Descriptor::Type::Count == Descriptor::Type::InputAttachment + 1);

namespace ShaderStageFlagBits
{
enum Type
{
    Vertex = 1 << 0,
    Fragment = 1 << 1,

    Max = 0x7FFFFFFF
};
}
using ShaderStageFlags = u32;

HANDLE_TYPE(DescriptorSetLayout);
HANDLE_TYPE(DescriptorPool);
HANDLE_TYPE(DescriptorSet);

struct DescriptorSetLayoutDesc
{
    u32 binding = 0;
    Descriptor::Type descriptorType = Descriptor::Type::Max;
    u32 descriptorCount = 0;
    ShaderStageFlags shaderStageFlags = 0;
};
struct DescriptorSetLayoutCreationParams
{
    DynamicArray<DescriptorSetLayoutDesc> bindings;
};

struct DescriptorPoolSize
{
    Descriptor::Type poolType = Descriptor::Max;
    u32 size = 0;
};

struct DescriptorPoolCreationParams
{
    DynamicArray<DescriptorPoolSize> poolSizes;
};

struct DescriptorSetAllocParams
{
    DescriptorSetLayout layout;
};

NODISCARD APPARITION_API DescriptorSetLayout CreateDescriptorSetLayout(Device device, const DescriptorSetLayoutCreationParams& params);
APPARITION_API void DestroyDescriptorSetLayout(DescriptorSetLayout descriptorSetLayout);

NODISCARD APPARITION_API DescriptorPool CreateDescriptorPool(Device device, const DescriptorPoolCreationParams& params);
APPARITION_API void DestroyDescriptorPool(DescriptorPool descriptorPool);

NODISCARD APPARITION_API DescriptorSet AllocateDescriptorSet(DescriptorPool descriptorPool, const DescriptorSetAllocParams& allocParams);
APPARITION_API void AllocateDescriptorSets(DescriptorPool descriptorPool, const DynamicArray<DescriptorSetAllocParams>& allocParams);
APPARITION_API void FreeDescriptorSet(DescriptorSet descriptorSet);
APPARITION_API void FreeDescriptorSets(const DynamicArray<DescriptorSet> descriptorSets);

HANDLE_TYPE(Sampler);

namespace SamplerAddressMode
{
enum Type
{
    Repeat,
    Clamp,
    Mirror
};
}

namespace SamplerFilter
{
enum Type
{
    Nearest,
    Linear
};
}

namespace SamplerMipmapMode
{
enum Type
{
    Nearest,
    Linear
};
}

struct SamplerCreationParams
{
    SamplerFilter::Type filter = SamplerFilter::Linear;
    SamplerAddressMode::Type addressModeU = SamplerAddressMode::Clamp;
    SamplerAddressMode::Type addressModeV = SamplerAddressMode::Clamp;
    SamplerMipmapMode::Type mipMode = SamplerMipmapMode::Linear;
    f32 maxAnisotropy = 0;
    f32 minLod = 0;
    f32 maxLod = 1;
};

NODISCARD APPARITION_API Sampler CreateSampler(Device device, const SamplerCreationParams& params);
APPARITION_API void DestroySampler(Sampler sampler);

// TEMPORARILY HERE
APPARITION_API VkSampler GetVulkanHandle(Sampler samplerHandle);

// Update Descriptor Set functionality
struct BufferDescriptorInfo
{
    Buffer buffer = { InvalidHandle };
    u64 offset = 0;
    u64 range = 0;
};

struct ImageDescriptorInfo
{
    Sampler sampler = { InvalidHandle };
    ImageView imageView = { InvalidHandle };
    ImageAccess::Type access = ImageAccess::Undefined;
};

struct UpdateDescriptorSetDesc
{
    DescriptorSet descriptorSet = { InvalidHandle };
    u32 setBinding = 0;
    Descriptor::Type descriptorType = Descriptor::Max;
    ImageDescriptorInfo* imageDescriptor = nullptr;
    BufferDescriptorInfo* bufferDescriptor = nullptr;
};

APPARITION_API void UpdateDescriptorSets(const DynamicArray<UpdateDescriptorSetDesc>& descriptorSetUpdates);

}