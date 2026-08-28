
#include "DescriptorHeap.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"
#include "Internal/HandleDefinitions.h"
#include "Internal/VulkanInfos.h"

namespace Apparition
{
AptnSamplerHeap CreateSamplerHeap(AptnDevice device, const AptnSamplerHeapCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateSamplerHeap(device, params);
}

void DestroySamplerHeap(AptnSamplerHeap descriptorHeap)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroySamplerHeap(descriptorHeap);
}

AptnResourceHeap CreateResourceHeap(AptnDevice device, const AptnResourceHeapCreationParams& params)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    return deviceManager.CreateResourceHeap(device, params);
}

void DestroyResourceHeap(AptnResourceHeap resourceHeap)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.DestroyResourceHeap(resourceHeap);

}

void WriteSamplerDescriptor(AptnSamplerHeap samplerHeap, const AptnSamplerDescriptor& samplerDescriptor)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.WriteSamplerDescriptors(samplerHeap, 1, &samplerDescriptor);
}

void WriteSamplerDescriptors(AptnSamplerHeap samplerHeap, u64 samplerDescriptorCount, const AptnSamplerDescriptor* samplerDescriptors)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.WriteSamplerDescriptors(samplerHeap, samplerDescriptorCount, samplerDescriptors);
}

void CommitSamplerDescriptors(AptnSamplerHeap samplerHeap)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.CommitSamplerDescriptors(samplerHeap);
}

void WriteBufferAddressDescriptor(AptnResourceHeap resourceHeap, const AptnBufferAddressDescriptor& bufferAddrDescriptor)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.WriteBufferAddressDescriptor(resourceHeap, bufferAddrDescriptor);
}

void WriteImageResourceDescriptor(AptnResourceHeap resourceHeap, const AptnImageDescriptor& imageDescriptor)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.WriteImageResourceDescriptor(resourceHeap, imageDescriptor);
}

void WriteBufferAddressDescriptors(AptnResourceHeap resourceHeap, const AptnBufferAddressDescriptor* bufferAddrDescriptors, u64 descriptorCount)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.WriteBufferAddressDescriptors(resourceHeap, bufferAddrDescriptors, descriptorCount);
}

void WriteImageResourceDescriptor(AptnResourceHeap resourceHeap, const AptnImageDescriptor* imageDescriptors, u64 descriptorCount)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.WriteImageResourceDescriptor(resourceHeap, imageDescriptors, descriptorCount);
}

void CommitResourceDescriptors(AptnResourceHeap resourceHeap)
{
    Assert(apparition.deviceManager);
    DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.CommitResourceDescriptors(resourceHeap);
}

}