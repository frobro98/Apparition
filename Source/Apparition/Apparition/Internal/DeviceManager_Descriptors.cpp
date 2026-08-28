
#include "DeviceManager.h"

#include "Apparition/DescriptorSet.h"

#include "ApparitionInternals.h"
#include "Conversions.h"
#include "ImageFormatConversion.h"
#include "Memory/MemoryCore.hpp"
#include "VulkanInfos.h"

AptnDescriptorSetLayout DeviceManager::CreateDescriptorSetLayout(AptnDevice device, const AptnDescriptorSetLayoutCreationParams& params)
{
    DynamicArray<VkDescriptorSetLayoutBinding> bindings;
    bindings.Reserve(params.bindings.Size());
    for (const AptnDescriptorSetLayoutDesc& desc : params.bindings)
    {
        VkDescriptorSetLayoutBinding binding
        {
            .binding = desc.binding,
            .descriptorType = ApparitionDescriptorTypeToVk(desc.descriptorType),
            .descriptorCount = desc.descriptorCount,
            .stageFlags = ApparitionShaderFlagsToVk(desc.shaderStageFlags)
        };
        bindings.Add(binding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = bindings.Size(),
        .pBindings = bindings.GetData()
    };

    DeviceInternal& deviceInternal = DeviceInternalFrom(device);
    
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    VkResult result = vkCreateDescriptorSetLayout(deviceInternal.device, &layoutInfo, nullptr, &layout);
    CHECK_VK(result);
    if (result == VK_SUCCESS)
    {
        const DescriptorSetLayoutInternal layoutInternal
        {
            .descriptorSetLayout = layout
        };
        const u32 handleIndex = PopFreeHandleIndex(deviceInternal.descriptorSetLayoutHandlePools);
        if (handleIndex != InvalidHandleIndex)
        {
            GetDescriptorSetLayoutInternalFromIndex(deviceInternal, handleIndex) = layoutInternal;

            const u32 indexGeneration = GetHandleGeneration(deviceInternal.descriptorSetLayoutHandlePools, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            const u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
                | (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return AptnDescriptorSetLayout{ handleData };
        }
    }

    return { AptnInvalidHandle };
}

void DeviceManager::DestroyDescriptorSetLayout(AptnDescriptorSetLayout descriptorSetLayout)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(descriptorSetLayout);
    DescriptorSetLayoutInternal& dsLayoutInternal = GetDescriptorSetLayoutInternal(descriptorSetLayout);
    vkDestroyDescriptorSetLayout(deviceInternal.device, dsLayoutInternal.descriptorSetLayout, nullptr);

    PushFreedHandleIndex(deviceInternal.descriptorSetLayoutHandlePools, GetHandleIndex(descriptorSetLayout));
}

AptnDescriptorPool DeviceManager::CreateDescriptorPool(AptnDevice device, const AptnDescriptorPoolCreationParams& params)
{
    u32 maxSets = 0;
    DynamicArray<VkDescriptorPoolSize> poolSizes(params.poolSizes.Size());
    for (u32 i = 0; i < params.poolSizes.Size(); ++i)
    {
        VkDescriptorType type = ApparitionDescriptorTypeToVk(params.poolSizes[i].poolType);
        u32 descriptorCount = params.poolSizes[i].size;
        maxSets += descriptorCount;
        VkDescriptorPoolSize poolSize
        {
            .type = type,
            .descriptorCount = descriptorCount
        };
        poolSizes[i] = poolSize;
    }

    DeviceInternal& deviceInternal = DeviceInternalFrom(device);

    VkDescriptorPoolCreateInfo poolInfo
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = maxSets,
        .poolSizeCount = poolSizes.Size(),
        .pPoolSizes = poolSizes.GetData()
    };
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    VkResult result = vkCreateDescriptorPool(deviceInternal.device, &poolInfo, nullptr, &descriptorPool);
    CHECK_VK(result);
    if (result == VK_SUCCESS)
    {
        const DescriptorPoolInternal poolInternal
        {
            .descriptorPool = descriptorPool
        };
        const u32 handleIndex = PopFreeHandleIndex(deviceInternal.descriptorPoolHandlePools);
        if (handleIndex != InvalidHandleIndex)
        {
            GetDescriptorPoolInternalFromIndex(deviceInternal, handleIndex) = poolInternal;

            const u32 indexGeneration = GetHandleGeneration(deviceInternal.descriptorPoolHandlePools, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            const u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
                | (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return AptnDescriptorPool{ handleData };
        }
    }

    return { AptnInvalidHandle };
}

void DeviceManager::DestroyDescriptorPool(AptnDescriptorPool descriptorPool)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(descriptorPool);
    DescriptorPoolInternal& descriptorPoolInternal = GetDescriptorPoolInternal(descriptorPool);
    vkDestroyDescriptorPool(deviceInternal.device, descriptorPoolInternal.descriptorPool, nullptr);

    PushFreedHandleIndex(deviceInternal.descriptorPoolHandlePools, GetHandleIndex(descriptorPool));
}

AptnDescriptorSet DeviceManager::AllocateDescriptorSet(AptnDescriptorPool descriptorPool, const AptnDescriptorSetAllocParams& allocParams)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(descriptorPool);
    DescriptorPoolInternal& poolInternal = GetDescriptorPoolInternal(descriptorPool);
    DescriptorSetLayoutInternal& layoutInternal = GetDescriptorSetLayoutInternal(allocParams.layout);

    VkDescriptorSetAllocateInfo allocInfo
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = poolInternal.descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &layoutInternal.descriptorSetLayout
    };
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    VkResult result = vkAllocateDescriptorSets(deviceInternal.device, &allocInfo, &descriptorSet);
    CHECK_VK(result);
    if (result == VK_SUCCESS)
    {
        const DescriptorSetInternal descriptorSetInternal
        {
            .descriptorSet = descriptorSet
        };
        const u32 handleIndex = PopFreeHandleIndex(deviceInternal.descriptorSetHandlePools);
        if (handleIndex != InvalidHandleIndex)
        {
            GetDescriptorSetInternalFromIndex(deviceInternal, handleIndex) = descriptorSetInternal;

            const u32 indexGeneration = GetHandleGeneration(deviceInternal.descriptorSetHandlePools, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            const u64 handleData = ((u64)GetDeviceIndexFromHandle(descriptorPool) << DEVICE_INDEX_SHIFT)
                | ((u64)GetHandleIndex(descriptorPool) << POOL_INDEX_SHIFT)
                | (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return AptnDescriptorSet{ handleData };
        }
    }
    return { AptnInvalidHandle };
}

void DeviceManager::FreeDescriptorSet(AptnDescriptorSet descriptorSet)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(descriptorSet);
    u32 dsPoolIndex = GetResourcePoolIndexFromHandle(descriptorSet);
    DescriptorPoolInternal& dsPoolInternal = deviceInternal.descriptorPoolResources[dsPoolIndex];
    DescriptorSetInternal& dsInternal = GetDescriptorSetInternal(descriptorSet);
    VkResult result = vkFreeDescriptorSets(deviceInternal.device, dsPoolInternal.descriptorPool, 1, &dsInternal.descriptorSet);
    CHECK_VK(result);

    PushFreedHandleIndex(deviceInternal.descriptorSetHandlePools, GetHandleIndex(descriptorSet));
}

void DeviceManager::AllocateDescriptorSets(AptnDescriptorPool descriptorPool, const DynamicArray<AptnDescriptorSetAllocParams>& allocParams)
{
    UNUSED(descriptorPool, allocParams);
    //return { Apparition::InvalidHandle };
}

void DeviceManager::FreeDescriptorSets(const DynamicArray<AptnDescriptorSet> descriptorSets)
{
    UNUSED(descriptorSets);
}

void DeviceManager::UpdateDescriptorSets(const DynamicArray<AptnUpdateDescriptorSetDesc>& descriptorSetUpdates)
{
    if (!descriptorSetUpdates.IsEmpty())
    {
        const DeviceInternal& deviceInternal = GetDeviceInternal(descriptorSetUpdates[0].descriptorSet);

        DynamicArray<VkWriteDescriptorSet> writeDescriptorSets;
        writeDescriptorSets.Reserve(descriptorSetUpdates.Size());
        for (const AptnUpdateDescriptorSetDesc& updateDescriptorSetDesc : descriptorSetUpdates)
        {
            const DescriptorSetInternal& setInternal = GetDescriptorSetInternal(updateDescriptorSetDesc.descriptorSet);
            VkWriteDescriptorSet writeSet
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = setInternal.descriptorSet,
                .dstBinding = updateDescriptorSetDesc.setBinding,
                .descriptorCount = 1,
                .descriptorType = ApparitionDescriptorTypeToVk(updateDescriptorSetDesc.descriptorType)
            };
            Assert(updateDescriptorSetDesc.bufferDescriptor || updateDescriptorSetDesc.imageDescriptor);
            Assert(!(updateDescriptorSetDesc.bufferDescriptor && updateDescriptorSetDesc.imageDescriptor));

            if (updateDescriptorSetDesc.imageDescriptor)
            {
                
                AptnImageAccess access = updateDescriptorSetDesc.imageDescriptor->access;
                AptnImageView view = updateDescriptorSetDesc.imageDescriptor->imageView;
                AptnSampler sampler = updateDescriptorSetDesc.imageDescriptor->sampler;

                VkImageView vkView = IsValid(view) ? GetImageViewInternal(view).imageView : VK_NULL_HANDLE;
                VkSampler vkSampler = IsValid(sampler) ? GetSamplerInternal(sampler).sampler : VK_NULL_HANDLE;
                VkImageLayout layout = ApparitionImageAccessToVkLayout(access);

                VkDescriptorImageInfo imageDescriptor
                {
                    .sampler = vkSampler,
                    .imageView = vkView,
                    .imageLayout = layout
                };
                writeSet.descriptorCount = 1;
                writeSet.pImageInfo = &imageDescriptor;
            }
            else if (updateDescriptorSetDesc.bufferDescriptor)
            {
                AptnBuffer buffer = updateDescriptorSetDesc.bufferDescriptor->buffer;
                Assert(IsValid(buffer));
                VkBuffer vkBuffer = GetBufferInternal(buffer).buffer;

                VkDescriptorBufferInfo bufferDescriptor
                {
                    .buffer = vkBuffer,
                    .offset = updateDescriptorSetDesc.bufferDescriptor->offset,
                    .range = updateDescriptorSetDesc.bufferDescriptor->range
                };

                writeSet.descriptorCount = 1;
                writeSet.pBufferInfo = &bufferDescriptor;
            }

            writeDescriptorSets.Add(writeSet);
        }

        vkUpdateDescriptorSets(deviceInternal.device, writeDescriptorSets.Size(), writeDescriptorSets.GetData(), 0, nullptr);
    }
}

AptnSamplerHeap DeviceManager::CreateSamplerHeap(AptnDevice device, const AptnSamplerHeapCreationParams& params)
{
    DeviceInternal& deviceInternal = DeviceInternalFrom(device);
    
    const VkDeviceSize samplerDescriptorSize = deviceInternal.descriptorHeapProperties.samplerDescriptorSize;
    const VkDeviceSize samplerDescriptorAlignment = deviceInternal.descriptorHeapProperties.samplerDescriptorAlignment;
    const VkDeviceSize minSamplerHeapReservedRange = deviceInternal.descriptorHeapProperties.minSamplerHeapReservedRange;

    const VkDeviceSize descriptorSize = Align(samplerDescriptorSize, samplerDescriptorAlignment);
    const VkDeviceSize samplerHeapAlignment = deviceInternal.descriptorHeapProperties.samplerHeapAlignment;
    const VkDeviceSize heapSize = Align(descriptorSize * params.samplerCount + minSamplerHeapReservedRange, samplerHeapAlignment);

    VkBufferCreateInfo bufferInfo
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = heapSize,
        .usage = VK_BUFFER_USAGE_DESCRIPTOR_HEAP_BIT_EXT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
    };

    VmaAllocationCreateInfo allocCreateInfo
    {
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        .requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    VkBuffer heapBuffer;
    VmaAllocation vmaAllocation;
    VkResult result = vmaCreateBuffer(deviceInternal.allocator, &bufferInfo, &allocCreateInfo, &heapBuffer, &vmaAllocation, nullptr);
    CHECK_VK(result);
    void* mappedData = nullptr;
    result = vmaMapMemory(deviceInternal.allocator, vmaAllocation, &mappedData);
    CHECK_VK(result);

    VkBufferDeviceAddressInfo bdaInfo
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .buffer = heapBuffer
    };
    VkDeviceAddress heapDeviceAddress = vkGetBufferDeviceAddress(deviceInternal.device, &bdaInfo);
    Assert(heapDeviceAddress > 0);

    if (result == VK_SUCCESS)
    {
        SamplerHeapInternal samplerHeapInternal
        {
            .heapBuffer = heapBuffer,
            .vmaAllocation = vmaAllocation,
            .heapSize = heapSize,
            .pHeapStart = mappedData,
            .pHeapEnd = reinterpret_cast<u8*>(mappedData) + heapSize,
            .pSamplerCurrent = mappedData,
            .heapDeviceAddress = heapDeviceAddress,
            .samplerDescriptorSize = descriptorSize,
            .heapReservedRange = minSamplerHeapReservedRange
        };
        const u32 handleIndex = PopFreeHandleIndex(deviceInternal.samplerHeapHandlePool);
        if (handleIndex != InvalidHandleIndex)
        {
            GetSamplerHeapInternalFromIndex(deviceInternal, handleIndex) = samplerHeapInternal;

            u32 handleGeneration = GetHandleGeneration(deviceInternal.samplerHeapHandlePool, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
                | (((u64)handleGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return AptnSamplerHeap{ handleData };
        }
    }
    return { AptnInvalidHandle };
}

void DeviceManager::DestroySamplerHeap(AptnSamplerHeap samplerHeap)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(samplerHeap);
    SamplerHeapInternal& samplerHeapInternal = GetSamplerHeapInternal(samplerHeap);
    vmaUnmapMemory(deviceInternal.allocator, samplerHeapInternal.vmaAllocation);
    vmaDestroyBuffer(deviceInternal.allocator, samplerHeapInternal.heapBuffer, samplerHeapInternal.vmaAllocation);

    PushFreedHandleIndex(deviceInternal.samplerHeapHandlePool, GetHandleIndex(samplerHeap));
}

void DeviceManager::WriteSamplerDescriptors(AptnSamplerHeap samplerHeap, u64 samplerDescriptorCount, const AptnSamplerDescriptor* samplerDescriptors)
{
    SamplerHeapInternal& heapInternal = GetSamplerHeapInternal(samplerHeap);
    const uptr samplerEnd = reinterpret_cast<uptr>(heapInternal.pHeapEnd);
    const uptr samplerCurrent = reinterpret_cast<uptr>(heapInternal.pSamplerCurrent);
    const uptr samplerBytesLeft = samplerEnd - samplerCurrent;
    Assert((samplerCurrent + (heapInternal.samplerDescriptorSize * samplerDescriptorCount)) < samplerEnd);
    Assert((heapInternal.pendingSamplerDescriptors.Size() * heapInternal.samplerDescriptorSize) + (samplerDescriptorCount * heapInternal.samplerDescriptorSize) < samplerBytesLeft);

    heapInternal.pendingSamplerDescriptors.AddRange(samplerDescriptors, samplerDescriptorCount);
}

void DeviceManager::CommitSamplerDescriptors(AptnSamplerHeap samplerHeap)
{
    SamplerHeapInternal& heapInternal = GetSamplerHeapInternal(samplerHeap);
    if (!heapInternal.pendingSamplerDescriptors.IsEmpty())
    {
        DynamicArray<VkSamplerCreateInfo> samplerCreateInfos;
        DynamicArray<VkHostAddressRangeEXT> hostAddressRanges;
        samplerCreateInfos.Reserve(heapInternal.pendingSamplerDescriptors.Size());
        hostAddressRanges.Reserve(heapInternal.pendingSamplerDescriptors.Size());
        for (const AptnSamplerDescriptor& samplerDescriptor : heapInternal.pendingSamplerDescriptors)
        {
            samplerCreateInfos.Add(
                VkSamplerCreateInfo
                {
                    .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                    .magFilter = ApparitionFilterToVk(samplerDescriptor.filter),
                    .minFilter = ApparitionFilterToVk(samplerDescriptor.filter),
                    .mipmapMode = ApparitionMipModeToVk(samplerDescriptor.mipMode),
                    .addressModeU = ApparitionAddressModeToVk(samplerDescriptor.addressModeU),
                    .addressModeV = ApparitionAddressModeToVk(samplerDescriptor.addressModeV),
                    .addressModeW = ApparitionAddressModeToVk(samplerDescriptor.addressModeU),
                    .anisotropyEnable = samplerDescriptor.maxAnisotropy > 0.f || samplerDescriptor.maxAnisotropy < 0.f,
                    .maxAnisotropy = samplerDescriptor.maxAnisotropy,
                    .minLod = samplerDescriptor.minLod,
                    .maxLod = samplerDescriptor.maxLod,
                    .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
                });
            hostAddressRanges.Add(
                VkHostAddressRangeEXT
                {
                    .address = heapInternal.pSamplerCurrent,
                    .size = heapInternal.samplerDescriptorSize
                });

            const uptr samplerEnd = reinterpret_cast<uptr>(heapInternal.pHeapEnd);
            heapInternal.pSamplerCurrent = reinterpret_cast<u8*>(heapInternal.pSamplerCurrent) + heapInternal.samplerDescriptorSize;
            Assert(reinterpret_cast<uptr>(heapInternal.pSamplerCurrent) <= samplerEnd);
        }

        const DeviceInternal& deviceInternal = GetDeviceInternal(samplerHeap);
        const VkResult result = vkWriteSamplerDescriptorsEXT(deviceInternal.device, samplerCreateInfos.Size(), samplerCreateInfos.GetData(), hostAddressRanges.GetData());
        CHECK_VK(result);

        // Reset internal tracking
        heapInternal.pendingSamplerDescriptors.Clear();
    }
}

AptnResourceHeap DeviceManager::CreateResourceHeap(AptnDevice device, const AptnResourceHeapCreationParams& params)
{
    DeviceInternal& deviceInternal = DeviceInternalFrom(device);

    const VkDeviceSize bufferDescriptorAlignment = deviceInternal.descriptorHeapProperties.bufferDescriptorAlignment;
    const VkDeviceSize bufferDescriptorSize = Align(deviceInternal.descriptorHeapProperties.bufferDescriptorSize, bufferDescriptorAlignment);
    const VkDeviceSize imageDescriptorAlignment = deviceInternal.descriptorHeapProperties.imageDescriptorAlignment;
    const VkDeviceSize imageDescriptorSize = Align(deviceInternal.descriptorHeapProperties.imageDescriptorSize, imageDescriptorAlignment);
    const VkDeviceSize minResourceHeapReservedRange = deviceInternal.descriptorHeapProperties.minResourceHeapReservedRange;
    // Buffer resources are stored first, image resources are stored after
    const VkDeviceSize bufferDescriptorBlockSize = Align(params.bufferCount * bufferDescriptorSize, imageDescriptorAlignment);
    const VkDeviceSize imageDescriptorBlockSize = Align(params.imageCount * imageDescriptorSize, imageDescriptorAlignment);
    const VkDeviceSize resourceHeapAlignment = deviceInternal.descriptorHeapProperties.resourceHeapAlignment;
    const VkDeviceSize heapSize = Align(bufferDescriptorBlockSize + imageDescriptorBlockSize + minResourceHeapReservedRange, resourceHeapAlignment);

    VkBufferCreateInfo bufferInfo
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = heapSize,
        .usage = VK_BUFFER_USAGE_DESCRIPTOR_HEAP_BIT_EXT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
    };

    VmaAllocationCreateInfo allocCreateInfo
    {
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        .requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    VkBuffer heapBuffer;
    VmaAllocation vmaAllocation;
    VkResult result = vmaCreateBuffer(deviceInternal.allocator, &bufferInfo, &allocCreateInfo, &heapBuffer, &vmaAllocation, nullptr);
    CHECK_VK(result);
    void* mappedData = nullptr;
    result = vmaMapMemory(deviceInternal.allocator, vmaAllocation, &mappedData);
    CHECK_VK(result);

    VkBufferDeviceAddressInfo bdaInfo
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        .buffer = heapBuffer
    };
    VkDeviceAddress heapDeviceAddress = vkGetBufferDeviceAddress(deviceInternal.device, &bdaInfo);
    Assert(heapDeviceAddress > 0);

    if (result == VK_SUCCESS)
    {
        ResourceHeapInternal resourceHeapInternal
        {
            .heapBuffer = heapBuffer,
            .vmaAllocation = vmaAllocation,
            .heapSize = heapSize,
            .heapData = mappedData,
            .pHeapEnd = reinterpret_cast<u8*>(mappedData) + heapSize,
            .pBDACurrent = mappedData,
            .pImgCurrent = reinterpret_cast<u8*>(mappedData) + bufferDescriptorBlockSize,
            .heapDeviceAddress = heapDeviceAddress,
            .bufferDescriptorSize = bufferDescriptorSize,
            .bufferDescriptorBlockSize = bufferDescriptorBlockSize,
            .imageDescriptorSize = imageDescriptorSize,
            .imageDescriptorBlockSize = imageDescriptorBlockSize,
            .heapReservedRange = minResourceHeapReservedRange
        };

        const u32 handleIndex = PopFreeHandleIndex(deviceInternal.resourceHeapHandlePool);
        if (handleIndex != InvalidHandleIndex)
        {
            GetResourceHeapInternalFromIndex(deviceInternal, handleIndex) = resourceHeapInternal;

            u32 handleGeneration = GetHandleGeneration(deviceInternal.resourceHeapHandlePool, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
                | (((u64)handleGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return AptnResourceHeap{ handleData };
        }
    }

    return { AptnInvalidHandle };
}

void DeviceManager::DestroyResourceHeap(AptnResourceHeap resourceHeap)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(resourceHeap);
    ResourceHeapInternal& resourceHeapInternal = GetResourceHeapInternal(resourceHeap);
    vmaUnmapMemory(deviceInternal.allocator, resourceHeapInternal.vmaAllocation);
    vmaDestroyBuffer(deviceInternal.allocator, resourceHeapInternal.heapBuffer, resourceHeapInternal.vmaAllocation);

    PushFreedHandleIndex(deviceInternal.resourceHeapHandlePool, GetHandleIndex(resourceHeap));
}

void DeviceManager::WriteBufferAddressDescriptor(AptnResourceHeap resourceHeap, const AptnBufferAddressDescriptor& bufferAddr)
{
    ResourceHeapInternal& heapInternal = GetResourceHeapInternal(resourceHeap);
    const uptr bdaStart = reinterpret_cast<uptr>(heapInternal.heapData);
    const uptr bdaEnd = bdaStart + heapInternal.bufferDescriptorBlockSize;
    const uptr bdaCurrent = reinterpret_cast<uptr>(heapInternal.pBDACurrent);
    const uptr bdaBytesLeft = bdaEnd - bdaCurrent;
    Assert((bdaCurrent + heapInternal.bufferDescriptorSize) < bdaEnd);
    Assert(heapInternal.pendingBufferDescriptors.Size() * heapInternal.bufferDescriptorSize < bdaBytesLeft);

    heapInternal.pendingBufferDescriptors.Add(bufferAddr);
}

void DeviceManager::WriteImageResourceDescriptor(AptnResourceHeap resourceHeap, const AptnImageDescriptor& imageDescriptor)
{
    ResourceHeapInternal& heapInternal = GetResourceHeapInternal(resourceHeap);
    const uptr imgStart = reinterpret_cast<uptr>(heapInternal.heapData) + heapInternal.bufferDescriptorBlockSize;
    const uptr imgEnd = imgStart + heapInternal.imageDescriptorBlockSize;
    const uptr imgCurrent = reinterpret_cast<uptr>(heapInternal.pImgCurrent);
    const uptr imgBytesLeft = imgEnd - imgCurrent;
    Assert((imgCurrent + heapInternal.imageDescriptorSize) < imgEnd);
    Assert(heapInternal.pendingImageDescriptors.Size() * heapInternal.imageDescriptorSize < imgBytesLeft);

    heapInternal.pendingImageDescriptors.Add(imageDescriptor);
}

void DeviceManager::WriteBufferAddressDescriptors(AptnResourceHeap resourceHeap, const AptnBufferAddressDescriptor* bufferAddrDescriptors, u64 descriptorCount)
{
    ResourceHeapInternal& heapInternal = GetResourceHeapInternal(resourceHeap);
    const uptr bdaStart = reinterpret_cast<size_t>(heapInternal.heapData);
    const uptr bdaEnd = bdaStart + heapInternal.bufferDescriptorBlockSize;
    const uptr bdaCurrent = reinterpret_cast<size_t>(heapInternal.pBDACurrent);
    const uptr bdaBytesLeft = bdaEnd - bdaCurrent;
    Assert((bdaCurrent + heapInternal.bufferDescriptorSize * descriptorCount) < bdaEnd);
    Assert(heapInternal.pendingBufferDescriptors.Size() * heapInternal.bufferDescriptorSize + heapInternal.bufferDescriptorSize * descriptorCount < bdaBytesLeft);

    heapInternal.pendingBufferDescriptors.AddRange(bufferAddrDescriptors, descriptorCount);
}

void DeviceManager::WriteImageResourceDescriptor(AptnResourceHeap resourceHeap, const AptnImageDescriptor* imageDescriptors, u64 descriptorCount)
{
    ResourceHeapInternal& heapInternal = GetResourceHeapInternal(resourceHeap);
    const uptr imgStart = reinterpret_cast<uptr>(heapInternal.heapData) + heapInternal.bufferDescriptorBlockSize;
    const uptr imgEnd = imgStart + heapInternal.imageDescriptorBlockSize;
    const uptr imgCurrent = reinterpret_cast<uptr>(heapInternal.pImgCurrent);
    const uptr imgBytesLeft = imgEnd - imgCurrent;
    const uptr imgPendingBytes = heapInternal.pendingImageDescriptors.Size() * heapInternal.imageDescriptorSize;
    const uptr imgAdditionalBytes = heapInternal.imageDescriptorSize * descriptorCount;
    Assert(imgPendingBytes + imgAdditionalBytes < imgBytesLeft);

    heapInternal.pendingImageDescriptors.AddRange(imageDescriptors, descriptorCount);
}

void DeviceManager::CommitResourceDescriptors(AptnResourceHeap resourceHeap)
{
    ResourceHeapInternal& heapInternal = GetResourceHeapInternal(resourceHeap);
    const u32 totalPendingDescriptors = heapInternal.pendingBufferDescriptors.Size() + heapInternal.pendingImageDescriptors.Size();
    if (totalPendingDescriptors > 0)
    {
        const uptr bdaStart = reinterpret_cast<uptr>(heapInternal.heapData);
        const uptr bdaEnd = bdaStart + heapInternal.bufferDescriptorBlockSize;
        const uptr imgStart = reinterpret_cast<uptr>(heapInternal.heapData) + heapInternal.bufferDescriptorBlockSize;
        const uptr imgEnd = imgStart + heapInternal.imageDescriptorBlockSize;

        DynamicArray<VkHostAddressRangeEXT> hostAddressRanges;
        DynamicArray<VkResourceDescriptorInfoEXT> resourceDescriptorInfos;
        hostAddressRanges.Reserve(totalPendingDescriptors);
        resourceDescriptorInfos.Reserve(totalPendingDescriptors);

        // Allocate space for DeviceAddressRange struct to keep them around for the Write call at the bottom
        DynamicArray<VkDeviceAddressRangeEXT> deviceAddressRanges;
        deviceAddressRanges.Reserve(heapInternal.pendingBufferDescriptors.Size());
        for (const AptnBufferAddressDescriptor& bufferDescriptor : heapInternal.pendingBufferDescriptors)
        {
            deviceAddressRanges.Add(
                VkDeviceAddressRangeEXT
                {
                    .address = bufferDescriptor.address,
                    .size = bufferDescriptor.size
                });
            hostAddressRanges.Add(
                VkHostAddressRangeEXT
                {
                    .address = heapInternal.pBDACurrent,
                    .size = heapInternal.bufferDescriptorSize
                });
            resourceDescriptorInfos.Add(
                VkResourceDescriptorInfoEXT
                {
                    .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
                    .type = ApparitionDescriptorTypeToVk(bufferDescriptor.type),
                    .data = {
                        .pAddressRange = &deviceAddressRanges.Last()
                    }
                });

            heapInternal.pBDACurrent = reinterpret_cast<u8*>(heapInternal.pBDACurrent) + heapInternal.bufferDescriptorSize;
            Assert(reinterpret_cast<uptr>(heapInternal.pBDACurrent) <= bdaEnd);
        }

        // Allocate space for VkImageViewCreateInfo and VkImageDescriptorInfoEXT structs to keep them around for the Write call below
        DynamicArray<VkImageViewCreateInfo> imageViewInfos;
        DynamicArray<VkImageDescriptorInfoEXT> imageDescriptors;
        imageViewInfos.Reserve(heapInternal.pendingImageDescriptors.Size());
        imageDescriptors.Reserve(heapInternal.pendingImageDescriptors.Size());
        for (const AptnImageDescriptor& imageDescriptor : heapInternal.pendingImageDescriptors)
        {
            const ImageInternal& imageInternal = GetImageInternal(imageDescriptor.image);
            imageViewInfos.Add(
                VkImageViewCreateInfo
                {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .image = imageInternal.image,
                    // TODO - Support multiple view types
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = ApparitionFormatToVk(imageDescriptor.format),
                    .subresourceRange =
                     {
                        .aspectMask = ApparitionImageAspectToVk(imageDescriptor.aspect),
                        .baseMipLevel = imageDescriptor.baseMipLevel,
                        .levelCount = imageDescriptor.mipCount,
                        .layerCount = 1
                     }
                });
            imageDescriptors.Add(
                VkImageDescriptorInfoEXT
                {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_DESCRIPTOR_INFO_EXT,
                    .pView = &imageViewInfos.Last(),
                    .layout = ApparitionImageAccessToVkLayout(imageDescriptor.access)
                });
            hostAddressRanges.Add(
                VkHostAddressRangeEXT
                {
                    .address = heapInternal.pImgCurrent,
                    .size = heapInternal.imageDescriptorSize
                });
            resourceDescriptorInfos.Add(
                VkResourceDescriptorInfoEXT
                {
                    .sType = VK_STRUCTURE_TYPE_RESOURCE_DESCRIPTOR_INFO_EXT,
                    .type = ApparitionDescriptorTypeToVk(imageDescriptor.type),
                    .data
                    {
                        .pImage = &imageDescriptors.Last()
                    }
                });

            heapInternal.pImgCurrent = reinterpret_cast<u8*>(heapInternal.pImgCurrent) + heapInternal.imageDescriptorSize;
            Assert(reinterpret_cast<uptr>(heapInternal.pImgCurrent) <= imgEnd);
        }

        const DeviceInternal& deviceInternal = GetDeviceInternal(resourceHeap);
        const VkResult result = vkWriteResourceDescriptorsEXT(deviceInternal.device, totalPendingDescriptors, resourceDescriptorInfos.GetData(), hostAddressRanges.GetData());
        CHECK_VK(result);

        // Reset internal arrays
        heapInternal.pendingBufferDescriptors.Clear();
        heapInternal.pendingImageDescriptors.Clear();
    }
}
