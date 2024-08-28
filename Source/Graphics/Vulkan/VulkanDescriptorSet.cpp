// Copyright 2020, Nathan Blane

#include "VulkanDescriptorSet.h"
#include "VulkanDevice.h"
#include "VulkanBufferAllocation.hpp"
#include "VulkanImageAllocation.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"
#include "VulkanDescriptorSetUtilities.hpp"

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDevice& device)
	: logicalDevice(device)
{
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	Assert(descriptorSetLayout != VK_NULL_HANDLE);
	vkDestroyDescriptorSetLayout(logicalDevice.GetNativeHandle(), descriptorSetLayout, nullptr);
}

void VulkanDescriptorSetLayout::AddDescriptorBinding(VkDescriptorType descriptorType, VkShaderStageFlags shaderStages, u32 binding)
{

	VkDescriptorSetLayoutBinding layoutBinding = {};
	if (!layoutBindings.TryFind(binding, layoutBinding))
	{
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.stageFlags = shaderStages;
		layoutBinding.binding = binding;
		layoutBinding.descriptorCount = 1;

		layoutBindings.Add(binding, layoutBinding);
	}
	else
	{
		layoutBinding.stageFlags |= shaderStages;
		layoutBindings[binding] = layoutBinding;
	}
}

void VulkanDescriptorSetLayout::BindLayout()
{
	Assert(descriptorSetLayout == VK_NULL_HANDLE);

	DynamicArray<VkDescriptorSetLayoutBinding> bindings = GetLayoutBindings();

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = bindings.Size();
	layoutInfo.pBindings = bindings.GetData();
	NOT_USED VkResult result = vkCreateDescriptorSetLayout(logicalDevice.GetNativeHandle(), &layoutInfo, nullptr, &descriptorSetLayout);
	CHECK_VK(result);
}

DynamicArray<VkDescriptorSetLayoutBinding> VulkanDescriptorSetLayout::GetLayoutBindings() const
{
	DynamicArray<VkDescriptorSetLayoutBinding> bindings;
	bindings.Reserve(layoutBindings.Size());
	for (auto bindingPair : layoutBindings)
	{
		bindings.Add(bindingPair.second);
	}
	return bindings;
}

VulkanDescriptorSet::VulkanDescriptorSet(const VulkanDevice& device, VulkanFence* fence, VkDescriptorSet ds, VulkanDescriptorSetLayout* layout)
	: descriptorSet(ds),
	logicalDevice(device),
	associatedLayout(layout),
	associatedFence(fence)
{
	u32 uniformDescriptors = 0;
	u32 samplerDescriptors = 0;
	u32 storageDescriptors = 0;
	DynamicArray<VkDescriptorSetLayoutBinding> layoutBindings = associatedLayout->GetLayoutBindings();
	for(auto& binding : layoutBindings)
	{
		// TODO - some of this information is set when updating. Compare what's set and then get rid of that info from here
		VkWriteDescriptorSet descriptorSetWriter = {};
		descriptorSetWriter.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorSetWriter.descriptorType = binding.descriptorType;
		if (binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
		{
			++uniformDescriptors;
		}
		else if (binding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
		{
			++storageDescriptors;
		}
		else if (binding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
		{
			++samplerDescriptors;
		}
		descriptorSetWriter.dstSet = descriptorSet;
		descriptorSetWriter.descriptorCount = binding.descriptorCount;
		descriptorSetWriter.dstBinding = binding.binding;
		descriptorWrites.Add(MOVE(descriptorSetWriter));
	}

	bufferInfos.Reserve(uniformDescriptors + storageDescriptors);
	imageInfos.Reserve(samplerDescriptors);
}
// 
// void VulkanDescriptorSet::SetUniformBufferInfo(const VulkanBuffer& uniformBuffer, uint32 bufferBinding)
// {
// 	for(auto& writer : descriptorWrites)
// 	{
// 		// TODO - Figure out how to test a failed binding
// 		if (writer.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER &&
// 			writer.dstBinding == bufferBinding)
// 		{
// 			if (writer.pBufferInfo == nullptr)
// 			{
// 				VkDescriptorBufferInfo bufferInfo = {};
// 				bufferInfo.buffer = uniformBuffer.handle;
// 				bufferInfo.offset = uniformBuffer.memory->alignedOffset;
// 				bufferInfo.range = uniformBuffer.memory->size;
// 				bufferInfos.Add(MOVE(bufferInfo));
// 				writer.pBufferInfo = &bufferInfos.Last();
// 				break;
// 			}
// 			else
// 			{
// 				VkDescriptorBufferInfo* bufferInfo = nullptr;
// 				for(auto& info : bufferInfos)
// 				{
// 					// TODO - This is sort of gross. I would like to figure out a way to not have to do this loop
// 					if (&info == writer.pBufferInfo)
// 					{
// 						bufferInfo = &info;
// 						break;
// 					}
// 				}
// 				bufferInfo->buffer = uniformBuffer.handle;
// 				bufferInfo->offset = uniformBuffer.memory->alignedOffset;
// 				bufferInfo->range = uniformBuffer.memory->size;
// 				break;
// 			}
// 		}
// 	}
// }
// 
// // TODO - this needs to be refactored. This is duplicate code when compared to the 
// void VulkanDescriptorSet::SetStorageBufferInfo(const VulkanBuffer& storageBuffer, uint32 bufferBinding)
// {
// 	for (auto& writer : descriptorWrites)
// 	{
// 		// TODO - Figure out how to test a failed binding
// 		if (writer.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER &&
// 			writer.dstBinding == bufferBinding)
// 		{
// 			if (writer.pBufferInfo == nullptr)
// 			{
// 				VkDescriptorBufferInfo bufferInfo = {};
// 				bufferInfo.buffer = storageBuffer.handle;
// 				bufferInfo.offset = 0;
// 				bufferInfo.range = storageBuffer.memory->size;
// 				bufferInfos.Add(std::move(bufferInfo));
// 				writer.pBufferInfo = &bufferInfos.Last();
// 				break;
// 			}
// 			else
// 			{
// 				VkDescriptorBufferInfo* bufferInfo = nullptr;
// 				for (auto& info : bufferInfos)
// 				{
// 					// TODO - This is sort of gross. I would like to figure out a way to not have to do this loop
// 					if (&info == writer.pBufferInfo)
// 					{
// 						bufferInfo = &info;
// 						break;
// 					}
// 				}
// 				bufferInfo->buffer = storageBuffer.handle;
// 				bufferInfo->offset = 0;
// 				bufferInfo->range = storageBuffer.memory->size;
// 				break;
// 			}
// 		}
// 	}
// }
// 
// void VulkanDescriptorSet::SetSampledTextureInfo(const VulkanTexture& texture, const TextureSampler& sampler, uint32 textureBinding)
// {
// 	for (auto& writer : descriptorWrites)
// 	{
// 		// TODO - Figure out how to test a failed binding
// 		if (writer.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER &&
// 			writer.dstBinding == textureBinding)
// 		{
// 			if (writer.pImageInfo == nullptr)
// 			{
// 				VkDescriptorImageInfo imageInfo = {};
// 				// TODO - There must be a better way to get the layout down to this level. Look up is required at this level as well...
// 				imageInfo.imageLayout = texture.image->format != VK_FORMAT_D32_SFLOAT_S8_UINT ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
// 				imageInfo.imageView = texture.imageView;
// 				imageInfo.sampler = sampler.nativeSampler;
// 				imageInfos.Add(std::move(imageInfo));
// 				writer.pImageInfo = &imageInfos.Last();
// 			}
// 			else
// 			{
// 				VkDescriptorImageInfo* imageInfo = nullptr;
// 				for (auto& info : imageInfos)
// 				{
// 					// TODO - This is sort of gross. I would like to figure out a way to not have to do this loop
// 					if (&info == writer.pImageInfo)
// 					{
// 						imageInfo = &info;
// 						break;
// 					}
// 				}
// 				// TODO - There must be a better way to get the layout down to this level. Look up is required at this level as well...
// 				imageInfo->imageLayout = texture.image->format != VK_FORMAT_D32_SFLOAT_S8_UINT ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
// 				imageInfo->imageView = texture.imageView;
// 				imageInfo->sampler = sampler.nativeSampler;
// 			}
// 		}
// 	}
// }

void VulkanDescriptorSet::UpdateDescriptorSet(const WriteDescriptorSet& writeDescriptorSet)
{
	DynamicArray<WriteDescriptor> descriptors = writeDescriptorSet.GetWriteDescriptors();
	for (u32 i = 0; i < descriptors.Size(); ++i)
	{
		WriteDescriptor& descriptor = descriptors[i];
		VkWriteDescriptorSet& writeDescriptor = descriptorWrites[i];
		if (descriptor.bufferDescriptor != nullptr)
		{
			Assert(writeDescriptor.descriptorType == descriptor.bufferDescriptor->descType);
			Assert(writeDescriptor.dstBinding == descriptor.bufferDescriptor->bindingIndex);
			writeDescriptor.pBufferInfo = &descriptor.bufferDescriptor->bufferInfo;
		}
		else if (descriptor.imageDescriptor != nullptr)
		{
			Assert(writeDescriptor.descriptorType == descriptor.imageDescriptor->descType);
			Assert(writeDescriptor.dstBinding == descriptor.imageDescriptor->bindingIndex);
			writeDescriptor.pImageInfo = &descriptor.imageDescriptor->imageInfo;
		}
		writeDescriptor.dstSet = descriptorSet;
	}

	vkUpdateDescriptorSets(logicalDevice.GetNativeHandle(), descriptorWrites.Size(), descriptorWrites.GetData(), 0, nullptr);
}

