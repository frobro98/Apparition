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

struct DescriptorSetLayoutCreationParams
{
    DynamicArray<Descriptor::Type> bindings;
};

struct DescriptorSetPoolCreationParams
{
    StaticArray<u32, Descriptor::Type::Max> poolSizes;
};

struct DescriptorSetAllocParams
{
    DescriptorSetLayout layout;
};

HANDLE_TYPE(DescriptorSetLayout);
HANDLE_TYPE(DescriptorSetPool);
HANDLE_TYPE(DescriptorSet);

}