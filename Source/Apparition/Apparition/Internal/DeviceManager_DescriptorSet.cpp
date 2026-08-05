
#include "DeviceManager.h"

#include "Apparition/DescriptorSet.h"

#include "Apparition/Internal/ApparitionInternals.h"
#include "Apparition/Internal/Conversions.h"
#include "Apparition/Internal/ImageFormatConversion.h"

DescriptorSetLayout DeviceManager::CreateDescriptorSetLayout(Device device, const DescriptorSetLayoutCreationParams& params)
{
    DynamicArray<VkDescriptorSetLayoutBinding> bindings;
    bindings.Reserve(params.bindings.Size());
    for (const Apparition::DescriptorSetLayoutDesc& desc : params.bindings)
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
            return DescriptorSetLayout{ handleData };
        }
    }

    return { Apparition::InvalidHandle };
}

void DeviceManager::DestroyDescriptorSetLayout(DescriptorSetLayout descriptorSetLayout)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(descriptorSetLayout);
    DescriptorSetLayoutInternal& dsLayoutInternal = GetDescriptorSetLayoutInternal(descriptorSetLayout);
    vkDestroyDescriptorSetLayout(deviceInternal.device, dsLayoutInternal.descriptorSetLayout, nullptr);

    PushFreedHandleIndex(deviceInternal.descriptorSetLayoutHandlePools, GetHandleIndex(descriptorSetLayout));
}

DescriptorPool DeviceManager::CreateDescriptorPool(Device device, const DescriptorPoolCreationParams& params)
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
            return DescriptorPool{ handleData };
        }
    }

    return { Apparition::InvalidHandle };
}

void DeviceManager::DestroyDescriptorPool(DescriptorPool descriptorPool)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(descriptorPool);
    DescriptorPoolInternal& descriptorPoolInternal = GetDescriptorPoolInternal(descriptorPool);
    vkDestroyDescriptorPool(deviceInternal.device, descriptorPoolInternal.descriptorPool, nullptr);

    PushFreedHandleIndex(deviceInternal.descriptorPoolHandlePools, GetHandleIndex(descriptorPool));
}

DescriptorSet DeviceManager::AllocateDescriptorSet(DescriptorPool descriptorPool, const DescriptorSetAllocParams& allocParams)
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
            return DescriptorSet{ handleData };
        }
    }
    return { Apparition::InvalidHandle };
}

void DeviceManager::FreeDescriptorSet(DescriptorSet descriptorSet)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(descriptorSet);
    u32 dsPoolIndex = GetResourcePoolIndexFromHandle(descriptorSet);
    DescriptorPoolInternal& dsPoolInternal = deviceInternal.descriptorPoolResources[dsPoolIndex];
    DescriptorSetInternal& dsInternal = GetDescriptorSetInternal(descriptorSet);
    VkResult result = vkFreeDescriptorSets(deviceInternal.device, dsPoolInternal.descriptorPool, 1, &dsInternal.descriptorSet);
    CHECK_VK(result);

    PushFreedHandleIndex(deviceInternal.descriptorSetHandlePools, GetHandleIndex(descriptorSet));
}

void DeviceManager::AllocateDescriptorSets(DescriptorPool descriptorPool, const DynamicArray<DescriptorSetAllocParams>& allocParams)
{
    UNUSED(descriptorPool, allocParams);
    //return { Apparition::InvalidHandle };
}

void DeviceManager::FreeDescriptorSets(const DynamicArray<DescriptorSet> descriptorSets)
{
    UNUSED(descriptorSets);
}

void DeviceManager::UpdateDescriptorSets(const DynamicArray<UpdateDescriptorSetDesc>& descriptorSetUpdates)
{
    if (!descriptorSetUpdates.IsEmpty())
    {
        const DeviceInternal& deviceInternal = GetDeviceInternal(descriptorSetUpdates[0].descriptorSet);

        DynamicArray<VkWriteDescriptorSet> writeDescriptorSets;
        writeDescriptorSets.Reserve(descriptorSetUpdates.Size());
        for (const UpdateDescriptorSetDesc& updateDescriptorSetDesc : descriptorSetUpdates)
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
                
                ImageAccess::Type access = updateDescriptorSetDesc.imageDescriptor->access;
                ImageView view = updateDescriptorSetDesc.imageDescriptor->imageView;
                Sampler sampler = updateDescriptorSetDesc.imageDescriptor->sampler;

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
                Buffer buffer = updateDescriptorSetDesc.bufferDescriptor->buffer;
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