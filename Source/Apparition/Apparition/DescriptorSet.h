#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/StaticArray.hpp"
#include "Apparition/ApparitionCore.h"
#include "Apparition/Device.h"
#include "Apparition/ApparitionAPI.hpp"

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

HANDLE_TYPE(DescriptorSetLayout);
HANDLE_TYPE(DescriptorPool);
HANDLE_TYPE(DescriptorSet);

struct DescriptorSetLayoutCreationParams
{
    DynamicArray<Descriptor::Type> bindings;
};

struct DescriptorPoolCreationParams
{
    StaticArray<u32, Descriptor::Type::Max> poolSizes;
};

struct DescriptorSetAllocParams
{
    DescriptorSetLayout layout;
};

NODISCARD APPARITION_API DescriptorSetLayout CreateDescriptorSetLayout(Device device, const DescriptorSetLayoutCreationParams& params);
APPARITION_API void DestroyDescriptorSetLayout(DescriptorSetLayout descriptorSetLayout);

NODISCARD APPARITION_API DescriptorPool CreateDescriptorPool(Device device, const DescriptorPoolCreationParams& params);
APPARITION_API void DestroyDescriptorPool(DescriptorPool descriptorPool);

NODISCARD APPARITION_API DescriptorSet AllocateDescriptorSet(DescriptorPool descriptorPool);
APPARITION_API void AllocateDescriptorSets(DescriptorPool descriptorPool, const DynamicArray<DescriptorSet>& descriptorSets);
void FreeDescriptorSet(DescriptorSet descriptorSet);
void FreeDescriptorSets(const DynamicArray<DescriptorSet> descriptorSets);

}