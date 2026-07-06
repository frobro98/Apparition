
#include "DeviceManager.h"

#include "Apparition/Buffer.h"
#include "ApparitionInternals.h"
#include "HandleDefinitions.h"
#include "VulkanInfos.h"

using namespace Apparition;

static VkBufferUsageFlags ApparitionToVkBufferUsage(BufferUsageFlags usageFlags)
{
    VkBufferUsageFlags vkUsageFlags = 0;
    if (usageFlags & BufferUsageFlagBits::TransferSrc)
    {
        vkUsageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }
    if (usageFlags & BufferUsageFlagBits::TransferDst)
    {
        vkUsageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    if (usageFlags & BufferUsageFlagBits::UniformBuffer)
    {
        vkUsageFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }
    if (usageFlags & BufferUsageFlagBits::StorageBuffer)
    {
        vkUsageFlags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }
    if (usageFlags & BufferUsageFlagBits::VertexBuffer)
    {
        vkUsageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
    if (usageFlags & BufferUsageFlagBits::IndexBuffer)
    {
        vkUsageFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }

    return vkUsageFlags;
}

Buffer DeviceManager::CreateBuffer(Device device, const BufferCreationParams& params)
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
            return Buffer{ handleData };
        }
    }

    return { InvalidHandle };
}

void DeviceManager::DestroyBuffer(Buffer buffer)
{
    u32 deviceIndex = GetDeviceIndexFromHandle(buffer);
    DeviceInternal& deviceInternal = deviceInternals[deviceIndex - 1];
    u32 handleIndex = GetHandleIndex(buffer);
    BufferInternal& bufferInternal = GetBufferInternalFromIndex(deviceInternal, handleIndex);
    vmaDestroyBuffer(deviceInternal.allocator, bufferInternal.buffer, bufferInternal.allocation);

    // Let the handle pool know this handle is freed
    PushFreedHandleIndex(deviceInternal.bufferResourceHandlePool, handleIndex);
}

ImageView DeviceManager::CreateImageView(Device /*device*/, const ImageViewCreationParams& /*params*/)
{
    return ImageView{};
}

void DeviceManager::DestroyImageView(ImageView /*imageView*/)
{
}

VkBuffer DeviceManager::GetBufferHandle(Buffer bufferHandle)
{
    u32 deviceIndex = GetDeviceIndexFromHandle(bufferHandle);
    DeviceInternal& deviceInternal = deviceInternals[deviceIndex - 1];
    u32 cmdBufferIndex = GetHandleIndex(bufferHandle);
    BufferInternal& bufferInternal = GetBufferInternalFromIndex(deviceInternal, cmdBufferIndex);

    return bufferInternal.buffer;
}
