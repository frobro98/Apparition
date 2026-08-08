
#include "DescriptorSet.h"

#include "Internal/ApparitionInternals.h"

namespace Apparition
{
AptnDescriptorSetLayout CreateDescriptorSetLayout(AptnDevice device, const AptnDescriptorSetLayoutCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateDescriptorSetLayout(device, params);
}

void DestroyDescriptorSetLayout(AptnDescriptorSetLayout descriptorSetLayout)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyDescriptorSetLayout(descriptorSetLayout);
}

AptnDescriptorPool CreateDescriptorPool(AptnDevice device, const AptnDescriptorPoolCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateDescriptorPool(device, params);
}

void DestroyDescriptorPool(AptnDescriptorPool descriptorPool)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyDescriptorPool(descriptorPool);
}

AptnDescriptorSet AllocateDescriptorSet(AptnDescriptorPool descriptorPool, const AptnDescriptorSetAllocParams& allocParams)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.AllocateDescriptorSet(descriptorPool, allocParams);
}

void AllocateDescriptorSets(AptnDescriptorPool descriptorPool, const DynamicArray<AptnDescriptorSetAllocParams>& allocParams)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.AllocateDescriptorSets(descriptorPool, allocParams);
}

void FreeDescriptorSet(AptnDescriptorSet descriptorSet)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.FreeDescriptorSet(descriptorSet);
}

void FreeDescriptorSets(const DynamicArray<AptnDescriptorSet> descriptorSets)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.FreeDescriptorSets(descriptorSets);
}

AptnSampler CreateSampler(AptnDevice device, const AptnSamplerCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateSampler(device, params);
}

void DestroySampler(AptnSampler sampler)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroySampler(sampler);
}

void UpdateDescriptorSets(const DynamicArray<AptnUpdateDescriptorSetDesc>& descriptorSetUpdates)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.UpdateDescriptorSets(descriptorSetUpdates);
}
}