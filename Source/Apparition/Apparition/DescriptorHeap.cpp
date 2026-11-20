
#include "DescriptorHeap.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/ApparitionHandleInternal.h"
#include "Internal/DeviceManager.h"

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

u64 QueryHeapCapacity(AptnSamplerHeap samplerHeap)
{
    Assert(apparition.deviceManager);
    const SamplerHeapInternal& heapInternal = GetSamplerHeapInternal(samplerHeap);
    return heapInternal.heapSize;
}

u64 QueryHeapCapacity(AptnResourceHeap resourceHeap)
{
    Assert(apparition.deviceManager);
    const ResourceHeapInternal& heapInternal = GetResourceHeapInternal(resourceHeap);
    return heapInternal.heapSize;
}

u64 QuerySamplerDescriptorCount(AptnSamplerHeap samplerHeap)
{
    Assert(apparition.deviceManager);
    const SamplerHeapInternal& heapInternal = GetSamplerHeapInternal(samplerHeap);
    const uptr start = reinterpret_cast<uptr>(heapInternal.pHeapStart);
    const uptr current = reinterpret_cast<uptr>(heapInternal.pSamplerCurrent);
    return (current - start) / heapInternal.samplerDescriptorSize;
}

u64 QueryBufferDescriptorCount(AptnResourceHeap resourceHeap)
{
    u64 unusedCount = 0;
    u64 bufferDescriptorCount = 0;
    QueryResourceHeapDescriptorCount(resourceHeap, bufferDescriptorCount, unusedCount);
    return bufferDescriptorCount;
}

u64 QueryImageDescriptorCount(AptnResourceHeap resourceHeap)
{
    u64 unusedCount = 0;
    u64 imageDescriptorCount = 0;
    QueryResourceHeapDescriptorCount(resourceHeap, unusedCount, imageDescriptorCount);
    return imageDescriptorCount;
}

void QueryResourceHeapDescriptorCount(AptnResourceHeap resourceHeap, u64& bufferDescriptorCount, u64& imageDescriptorCount)
{
    Assert(apparition.deviceManager);
    const ResourceHeapInternal& heapInternal = GetResourceHeapInternal(resourceHeap);

    const uptr bufferStart = reinterpret_cast<uptr>(heapInternal.heapData);
    const uptr bufferCurrent = reinterpret_cast<uptr>(heapInternal.pBDACurrent);
    const uptr imageStart = bufferStart + heapInternal.bufferDescriptorBlockSize;
    const uptr imageCurrent = reinterpret_cast<uptr>(heapInternal.pImgCurrent);
    bufferDescriptorCount = (bufferCurrent - bufferStart) / heapInternal.bufferDescriptorSize;
    imageDescriptorCount = (imageCurrent - imageStart) / heapInternal.imageDescriptorSize;
}

u32 GetBufferDescriptorsStartIndex(AptnResourceHeap resourceHeap)
{
    Assert(apparition.deviceManager);
    const ResourceHeapInternal& heapInternal = GetResourceHeapInternal(resourceHeap);

    // TODO - This is in preparation to prevent bad packing (small descriptors leading to required padding)
    const uptr bufferStart = reinterpret_cast<uptr>(heapInternal.heapData);
    const u32 bytesFromHeapStart = bufferStart - reinterpret_cast<uptr>(heapInternal.heapData);
    return bytesFromHeapStart / heapInternal.bufferDescriptorSize;
}

u32 GetImageDescriptorsStartIndex(AptnResourceHeap resourceHeap)
{
    Assert(apparition.deviceManager);
    const ResourceHeapInternal& heapInternal = GetResourceHeapInternal(resourceHeap);

    const uptr imageStart = reinterpret_cast<uptr>(heapInternal.heapData) + heapInternal.bufferDescriptorBlockSize;
    const u32 bytesFromHeapStart = imageStart - reinterpret_cast<uptr>(heapInternal.heapData);
    return bytesFromHeapStart / heapInternal.imageDescriptorSize;
}

u64 GetSamplerDescriptorSize(AptnSamplerHeap samplerHeap)
{
    Assert(apparition.deviceManager);
    const SamplerHeapInternal& heapInternal = GetSamplerHeapInternal(samplerHeap);
    return heapInternal.samplerDescriptorSize;
}

u64 GetBufferDescriptorSize(AptnResourceHeap resourceHeap)
{
    Assert(apparition.deviceManager);
    const ResourceHeapInternal& heapInternal = GetResourceHeapInternal(resourceHeap);
    return heapInternal.bufferDescriptorSize;
}

u64 GetImageDescriptorSize(AptnResourceHeap resourceHeap)
{
    Assert(apparition.deviceManager);
    const ResourceHeapInternal& heapInternal = GetResourceHeapInternal(resourceHeap);
    return heapInternal.imageDescriptorSize;
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