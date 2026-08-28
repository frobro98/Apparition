
#include "DeviceManager.h"

#include "Apparition/Buffer.h"
#include "Apparition/Image.h"

#include "ApparitionInternals.h"
#include "Conversions.h"
#include "HandleDefinitions.h"
#include "ImageFormatConversion.h"
#include "VulkanInfos.h"

AptnBuffer DeviceManager::CreateBuffer(AptnDevice device, const AptnBufferCreationParams& params)
{
    DeviceInternal& deviceInternal = DeviceInternalFrom(device);

    VkBufferCreateInfo bufferInfo = {};
    Vk::ZeroInfoStruct(bufferInfo, VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);
    bufferInfo.usage = ApparitionToVkBufferUsage(params.usage);
    bufferInfo.size = params.size;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    if (params.supportsMappedMemory)
    {
        allocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    }

    VkBuffer buffer;
    VmaAllocation vmaAllocation;
    VkResult result = vmaCreateBuffer(deviceInternal.allocator, &bufferInfo, &allocCreateInfo, &buffer, &vmaAllocation, nullptr);
    CHECK_VK(result);
    if (result == VK_SUCCESS)
    {
        BufferInternal bufferInternal = {};
        bufferInternal.buffer = buffer;
        bufferInternal.allocation = vmaAllocation;
        bufferInternal.isMappable = params.supportsMappedMemory;
        if (HasAnyEnumFlags(params.usage, AptnBufferUsageFlags::ShaderDeviceAddress))
        {
            VkBufferDeviceAddressInfoKHR deviceAddrInfo
            {
                .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
                .buffer = buffer
            };
            bufferInternal.bufferDeviceAddress = vkGetBufferDeviceAddress(deviceInternal.device, &deviceAddrInfo);
        }

        u32 handleIndex = PopFreeHandleIndex(deviceInternal.bufferResourceHandlePool);
        if (handleIndex != InvalidHandleIndex)
        {
            // TODO - this kinda is stinky to me, don't assign to the return of a function...
            GetBufferInternalFromIndex(deviceInternal, handleIndex) = bufferInternal;

            u32 handleGeneration = GetHandleGeneration(deviceInternal.bufferResourceHandlePool, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
                | (((u64)handleGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return AptnBuffer{ handleData };
        }
    }

    return { AptnInvalidHandle };
}

void DeviceManager::DestroyBuffer(AptnBuffer buffer)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(buffer);
    BufferInternal& bufferInternal = GetBufferInternal(buffer);
    vmaDestroyBuffer(deviceInternal.allocator, bufferInternal.buffer, bufferInternal.allocation);

    // TODO - Cleanup handle data?
    
    // Let the handle pool know this handle is freed
    PushFreedHandleIndex(deviceInternal.bufferResourceHandlePool, GetHandleIndex(buffer));
}

AptnImage DeviceManager::CreateImage(AptnDevice device, const AptnImageCreationParams& params)
{
    DeviceInternal& deviceInternal = DeviceInternalFrom(device);
    VkImageCreateInfo imageInfo
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = ApparitionFormatToVk(params.format),
        .extent = {.width = params.width, .height = params.height, .depth = 1 },
        .mipLevels = params.mipLevels,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = ApparitionImageUsageToVk(params.usageFlags),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

    VkImage vkImage = VK_NULL_HANDLE;
    VmaAllocation vmaAllocation;
    VkResult result = vmaCreateImage(deviceInternal.allocator, &imageInfo, &allocCreateInfo, &vkImage, &vmaAllocation, nullptr);
    CHECK_VK(result);
    if (result == VK_SUCCESS)
    {
        // Intial Image Data Setup
        ImageInternal imageInternal
        {
            .image = vkImage,
            .allocation = vmaAllocation,
            .format = params.format
        };

        // Allocate specific access per mip level
        imageInternal.access.Resize(params.mipLevels);
        for (auto& access : imageInternal.access)
        {
            access = AptnImageAccess::Undefined;
        }

        u32 handleIndex = PopFreeHandleIndex(deviceInternal.imageResourceHandlePool);
        if (handleIndex != InvalidHandleIndex)
        {
            // TODO - this kinda is stinky to me, don't assign to the return of a function...
            GetImageInternalFromIndex(deviceInternal, handleIndex) = imageInternal;

            u32 handleGeneration = GetHandleGeneration(deviceInternal.imageResourceHandlePool, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
                | (((u64)handleGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return AptnImage{ handleData };
        }
    }

    return { AptnInvalidHandle };
}

void DeviceManager::DestroyImage(AptnImage image)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(image);
    ImageInternal& imageInternal = GetImageInternal(image);
    vmaDestroyImage(deviceInternal.allocator, imageInternal.image, imageInternal.allocation);

    // TODO - Cleanup handle data?

    PushFreedHandleIndex(deviceInternal.imageResourceHandlePool, GetHandleIndex(image));
}

AptnImageView DeviceManager::CreateImageView(AptnImage image, const AptnImageViewCreationParams& params)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(image);
    ImageInternal& imageInternal = GetImageInternal(image);
    VkImageViewCreateInfo viewInfo
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = imageInternal.image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = ApparitionFormatToVk(params.format),
        .components = 
        { 
            .r = VK_COMPONENT_SWIZZLE_IDENTITY, 
            .g = VK_COMPONENT_SWIZZLE_IDENTITY, 
            .b = VK_COMPONENT_SWIZZLE_IDENTITY, 
            .a = VK_COMPONENT_SWIZZLE_IDENTITY 
        },
        .subresourceRange
        {
            .aspectMask = ApparitionImageAspectToVk(params.aspect),
            .baseMipLevel = params.baseMipLevel,
            .levelCount = params.mipCount,
            .layerCount = 1
        }
    };

    VkImageView imageView = VK_NULL_HANDLE;
    VkResult result = vkCreateImageView(deviceInternal.device, &viewInfo, nullptr, &imageView);
    CHECK_VK(result);
    if (result == VK_SUCCESS)
    {
        ImageViewInternal imageViewInternal
        {
            .imageView = imageView,
            .imageIndex = GetHandleIndex(image)
        };

        u32 handleIndex = PopFreeHandleIndex(deviceInternal.imageViewResourceHandlePool);
        if (handleIndex != InvalidHandleIndex)
        {
            // TODO - this kinda is stinky to me, don't assign to the return of a function...
            GetImageViewInternalFromIndex(deviceInternal, handleIndex) = imageViewInternal;

            u32 handleGeneration = GetHandleGeneration(deviceInternal.imageViewResourceHandlePool, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            u64 handleData = ((u64)GetDeviceIndexFromHandle(image) << DEVICE_INDEX_SHIFT)
                | (((u64)handleGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return AptnImageView{ handleData };
        }
    }

    return { AptnInvalidHandle };
}

void DeviceManager::DestroyImageView(AptnImageView imageView)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(imageView);
    ImageViewInternal& imageViewInternal = GetImageViewInternal(imageView);
    vkDestroyImageView(deviceInternal.device, imageViewInternal.imageView, nullptr);

    PushFreedHandleIndex(deviceInternal.imageViewResourceHandlePool, GetHandleIndex(imageView));
}

AptnSampler DeviceManager::CreateSampler(AptnDevice device, const AptnSamplerCreationParams& params)
{
    VkSamplerCreateInfo samplerInfo
    {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = ApparitionFilterToVk(params.filter),
        .minFilter = ApparitionFilterToVk(params.filter),
        .mipmapMode = ApparitionMipModeToVk(params.mipMode),
        .addressModeU = ApparitionAddressModeToVk(params.addressModeU),
        .addressModeV = ApparitionAddressModeToVk(params.addressModeV),
        .addressModeW = ApparitionAddressModeToVk(params.addressModeU),
        .anisotropyEnable = params.maxAnisotropy > 0.f || params.maxAnisotropy < 0.f,
        .maxAnisotropy = params.maxAnisotropy,
        .minLod = params.minLod,
        .maxLod = params.maxLod,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
    };

    DeviceInternal& deviceInternal = DeviceInternalFrom(device);

    VkSampler sampler = VK_NULL_HANDLE;
    VkResult result = vkCreateSampler(deviceInternal.device, &samplerInfo, nullptr, &sampler);
    CHECK_VK(result);
    if (result == VK_SUCCESS)
    {
        SamplerInternal samplerInternal
        {
            .sampler = sampler
        };

        u32 handleIndex = PopFreeHandleIndex(deviceInternal.samplerResourceHandlePool);
        if (handleIndex != InvalidHandleIndex)
        {
            // TODO - this kinda is stinky to me, don't assign to the return of a function...
            GetSamplerInternalFromIndex(deviceInternal, handleIndex) = samplerInternal;

            u32 handleGeneration = GetHandleGeneration(deviceInternal.samplerResourceHandlePool, handleIndex);
            // TODO(nblane): this MUST be moved so that it can be reused
            u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
                | (((u64)handleGeneration) << RESOURCE_GEN_SHIFT)
                | (handleIndex & RESOURCE_INDEX_MASK);
            return AptnSampler{ handleData };
        }
    }

    return { AptnInvalidHandle };
}

void DeviceManager::DestroySampler(AptnSampler sampler)
{
    DeviceInternal& deviceInternal = GetDeviceInternal(sampler);
    SamplerInternal& samplerInternal = GetSamplerInternal(sampler);
    vkDestroySampler(deviceInternal.device, samplerInternal.sampler, nullptr);

    PushFreedHandleIndex(deviceInternal.samplerResourceHandlePool, GetHandleIndex(sampler));
}
