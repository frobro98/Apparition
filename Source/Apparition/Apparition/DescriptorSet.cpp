
#include "DescriptorSet.h"

#include "Internal/ApparitionInternals.h"

namespace Apparition
{
DescriptorSetLayout CreateDescriptorSetLayout(Device device, const DescriptorSetLayoutCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateDescriptorSetLayout(device, params);
}

void DestroyDescriptorSetLayout(DescriptorSetLayout descriptorSetLayout)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyDescriptorSetLayout(descriptorSetLayout);
}

DescriptorPool CreateDescriptorPool(Device device, const DescriptorPoolCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateDescriptorPool(device, params);
}

void DestroyDescriptorPool(DescriptorPool descriptorPool)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyDescriptorPool(descriptorPool);
}

DescriptorSet AllocateDescriptorSet(DescriptorPool descriptorPool, const DescriptorSetAllocParams& allocParams)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.AllocateDescriptorSet(descriptorPool, allocParams);
}

void AllocateDescriptorSets(DescriptorPool descriptorPool, const DynamicArray<DescriptorSetAllocParams>& allocParams)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.AllocateDescriptorSets(descriptorPool, allocParams);
}

void FreeDescriptorSet(DescriptorSet descriptorSet)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.FreeDescriptorSet(descriptorSet);
}

void FreeDescriptorSets(const DynamicArray<DescriptorSet> descriptorSets)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.FreeDescriptorSets(descriptorSets);
}

Sampler CreateSampler(Device device, const SamplerCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateSampler(device, params);
}

void DestroySampler(Sampler sampler)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroySampler(sampler);
}

VkSampler GetVulkanHandle(Sampler samplerHandle)
{
    Assert(false);
    UNUSED(samplerHandle);
    return nullptr;
}

void UpdateDescriptorSets(const DynamicArray<UpdateDescriptorSetDesc>& descriptorSetUpdates)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.UpdateDescriptorSets(descriptorSetUpdates);
}
}