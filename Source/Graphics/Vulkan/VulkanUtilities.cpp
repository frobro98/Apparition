// Copyright 2020, Nathan Blane

#include "VulkanUtilities.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanBufferAllocation.hpp"
#include "VulkanStagingBufferManager.hpp"

VkAccessFlags GetAccessFlagsFor(VkImageLayout layout)
{
	VkAccessFlags flags = 0;

	switch (layout)
	{
		case VK_IMAGE_LAYOUT_UNDEFINED:
		case VK_IMAGE_LAYOUT_GENERAL:
		{
			flags = 0;
		}break;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		{
			flags = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		}break;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		{
			flags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}break;
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
		{
			flags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		}break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		{
			flags = VK_ACCESS_SHADER_READ_BIT;
		}break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		{
			flags = VK_ACCESS_TRANSFER_READ_BIT;
		}break;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		{
			flags = VK_ACCESS_TRANSFER_WRITE_BIT;
		}break;
		case VK_IMAGE_LAYOUT_PREINITIALIZED:
		{
			flags = VK_ACCESS_HOST_READ_BIT;
		}break;
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		{
			flags = VK_ACCESS_MEMORY_READ_BIT;
		}break;
		default:
			Assert(false);
			break;
	}

	return flags;
}

VkPipelineStageFlags GetStageFor(VkImageLayout layout)
{
	VkPipelineStageFlags flags = 0;
	switch (layout)
	{
		case VK_IMAGE_LAYOUT_UNDEFINED:
		case VK_IMAGE_LAYOUT_GENERAL:
		case VK_IMAGE_LAYOUT_PREINITIALIZED:
		{
			flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		{
			flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		{
			flags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
		{
			flags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}break;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		{
			flags = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		}break;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		{
			flags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}break;
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		{
			flags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}break;
		default:
			Assert(false);
			break;
	}

	return flags;
}

bool PresentationSupported(VkPhysicalDevice gpu, u32 queueIndex)
{
	return vkGetPhysicalDeviceWin32PresentationSupportKHR((VkPhysicalDevice)gpu, queueIndex);
}

void ImageLayoutTransition(VulkanCommandBuffer& cmdBuffer, const VkImageSubresourceRange& resourceRange, VkImageLayout newLayout, const DynamicArray<VulkanImage*>& images)
{
	if (images.Size() > 0)
	{
		u32 imageCount = images.Size();
		DynamicArray<VkImageMemoryBarrier> imageBarriers;
		imageBarriers.Reserve(imageCount);
		VkPipelineStageFlags srcStageFlags = 0;
		VkPipelineStageFlags dstStageFlags = 0;
		for (u32 i = 0; i < imageCount; ++i)
		{
			VulkanImage& image = *images[i];
			if (image.layout != newLayout)
			{
				VkImageMemoryBarrier imageBarrier = {};
				imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageBarrier.image = image.handle;
				imageBarrier.oldLayout = image.layout;
				imageBarrier.newLayout = newLayout;
				imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageBarrier.subresourceRange = resourceRange;

				imageBarrier.srcAccessMask = GetAccessFlagsFor(image.layout);
				imageBarrier.dstAccessMask = GetAccessFlagsFor(newLayout);

				srcStageFlags |= GetStageFor(image.layout);
				dstStageFlags |= GetStageFor(newLayout);

				imageBarriers.Add(imageBarrier);

				image.layout = newLayout;
			}
		}

		if (!imageBarriers.IsEmpty())
		{
			Assert(srcStageFlags != 0);
			Assert(dstStageFlags != 0);

			cmdBuffer.ImageMemoryBarrier(
				srcStageFlags, dstStageFlags,
				0,
				imageBarriers.Size(), imageBarriers.GetData()
			);
		}
	}
}

void CopyToDeviceBuffer(VulkanCommandBuffer& cmdBuffer, const VulkanBuffer& stagingBuffer, const VulkanBuffer& dstBuffer)
{
	Assert(stagingBuffer.memory->size <= dstBuffer.memory->size);
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = dstBuffer.memory->size;

	cmdBuffer.CopyBuffer(stagingBuffer.handle, dstBuffer.handle, 1, &copyRegion);

}

void CopyStagingBufferToImage(VulkanCommandBuffer& cmdBuffer, const VulkanStagingBuffer& buffer, const VulkanImage& image)
{
	constexpr u32 numColorComponents = 4;
	DynamicArray<VkBufferImageCopy> regionsToCopy(image.mipmapLevels);
	u32 offset = 0;
	for (u32 i = 0; i < image.mipmapLevels; ++i)
	{
		u32 width = image.width >> i;
		u32 height = image.height >> i;

		VkBufferImageCopy region = {};
		region.bufferOffset = offset;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = i;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageExtent = { width, height, 1 };

		regionsToCopy[i] = region;

		offset += width * height * numColorComponents;
	}

	cmdBuffer.CopyBufferToImage(buffer.stagingHandle, image.handle,
		(VkImageLayout)(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL | VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
		regionsToCopy.Size(), regionsToCopy.GetData());
}

void CopyBufferToImage(VulkanCommandBuffer& cmdBuffer, const VulkanBuffer& buffer, const VulkanImage& image)
{
	constexpr u32 numColorComponents = 4;
	DynamicArray<VkBufferImageCopy> regionsToCopy(image.mipmapLevels);
	u32 offset = 0;
	for (u32 i = 0; i < image.mipmapLevels; ++i)
	{
		u32 width = image.width >> i;
		u32 height = image.height >> i;

		VkBufferImageCopy region = {};
		region.bufferOffset = offset;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = i;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageExtent = { width, height, 1 };

		regionsToCopy[i] = region;

		offset += width * height * numColorComponents;
	}

	cmdBuffer.CopyBufferToImage(buffer.handle, image.handle, 
		(VkImageLayout)(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL | VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
		regionsToCopy.Size(), regionsToCopy.GetData());
}

void CopyImage(VulkanCommandBuffer& cmdBuffer, const VulkanImage& srcImage, const VulkanImage& dstImage)
{
	//constexpr u32 numColorComponents = 4;
	DynamicArray<VkImageCopy> regionsToCopy(srcImage.mipmapLevels);
	for (u32 i = 0; i < srcImage.mipmapLevels; ++i)
	{
		u32 width = srcImage.width >> i;
		u32 height = srcImage.height >> i;

		VkImageCopy region = {};
		region.srcOffset = { 0, 0, 0 };
		region.srcSubresource.aspectMask = srcImage.format != VK_FORMAT_D32_SFLOAT_S8_UINT ? (u32)VK_IMAGE_ASPECT_COLOR_BIT : (u32)(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
		region.srcSubresource.mipLevel = i;
		region.srcSubresource.baseArrayLayer = 0;
		region.srcSubresource.layerCount = 1;

		region.dstOffset = { 0, 0, 0 };
		region.dstSubresource.aspectMask = dstImage.format != VK_FORMAT_D32_SFLOAT_S8_UINT ? (u32)VK_IMAGE_ASPECT_COLOR_BIT : (u32)(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
		region.dstSubresource.mipLevel = i;
		region.dstSubresource.baseArrayLayer = 0;
		region.dstSubresource.layerCount = 1;

		region.extent = { width, height, 1 };

		regionsToCopy[i] = region;
	}

	cmdBuffer.CopyImage(srcImage.handle, srcImage.layout,
		dstImage.handle, dstImage.layout,
		regionsToCopy.Size(), regionsToCopy.GetData());
}

VkDescriptorType MusaConstantToDescriptorType(ShaderConstant constant)
{
	switch (constant.bindingType)
	{
		case ShaderResourceType::StorageBuffer:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case ShaderResourceType::StorageDynamicBuffer:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		case ShaderResourceType::TextureSampler:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case ShaderResourceType::UniformBuffer:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case ShaderResourceType::UniformDynamicBuffer:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		default:
			Assert(false);
	}

	return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

VkFormat MusaFormatToVkFormat(ImageFormat format)
{
	switch (format)
	{
		case ImageFormat::RGB_8norm:
			return VK_FORMAT_R8G8B8_UNORM;
		case ImageFormat::RGB_8u:
			return VK_FORMAT_R8G8B8_UINT;
		case ImageFormat::RGB_16f:
			return VK_FORMAT_R16G16B16_SFLOAT;
		case ImageFormat::BGR_8u:
			return VK_FORMAT_B8G8R8_UNORM;
		case ImageFormat::RGBA_8norm:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case ImageFormat::RGBA_8u:
			return VK_FORMAT_R8G8B8A8_UINT;
		case ImageFormat::RGBA_16f:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case ImageFormat::BGRA_8norm:
			return VK_FORMAT_B8G8R8A8_UNORM;
		case ImageFormat::Gray_8u:
			return VK_FORMAT_R8_UNORM;
		case ImageFormat::BC1:
			return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
		case ImageFormat::BC3:
			return VK_FORMAT_BC3_UNORM_BLOCK;
		case ImageFormat::BC7:
			return VK_FORMAT_BC7_UNORM_BLOCK;
		case ImageFormat::D_32f:
			return VK_FORMAT_D32_SFLOAT;
		case ImageFormat::DS_32f_8u:
			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case ImageFormat::DS_24f_8u:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		case ImageFormat::Invalid:
		default:
			Assert(false);
	}

	return VK_FORMAT_UNDEFINED;
}

VkImageAspectFlags MusaFormatToVkAspect(ImageFormat format)
{
	switch (format)
	{
		case ImageFormat::DS_32f_8u:
			return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		case ImageFormat::D_32f:
			return VK_IMAGE_ASPECT_DEPTH_BIT;
		default:
			return VK_IMAGE_ASPECT_COLOR_BIT;
	}
}

VkFormat MusaInputsToVk(VertexInputType type)
{
	switch (type)
	{
		case VertexInputType::F32_1:
			return VK_FORMAT_R32_SFLOAT;
		case VertexInputType::F32_2:
			return VK_FORMAT_R32G32_SFLOAT;
		case VertexInputType::F32_3:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case VertexInputType::F32_4:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case VertexInputType::U32:
			return VK_FORMAT_R8G8B8A8_UNORM;
		default:
			return VK_FORMAT_UNDEFINED;
	}
}

VkPrimitiveTopology MusaTopologyToVk(PrimitiveTopology topology)
{
	switch (topology)
	{
		case PrimitiveTopology::TriangleList:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case PrimitiveTopology::TriangleStrip:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case PrimitiveTopology::TriangleFan:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		case PrimitiveTopology::LineList:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case PrimitiveTopology::LineStrip:
			return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case PrimitiveTopology::PointList:
			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		default:
			return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
	}
}

VkPolygonMode MusaPolyToVk(FillMode mode)
{
	switch (mode)
	{
		case FillMode::Full:
			return VK_POLYGON_MODE_FILL;
		case FillMode::Wireframe:
			return VK_POLYGON_MODE_LINE;
		case FillMode::Point:
			return VK_POLYGON_MODE_POINT;
		default:
			return VK_POLYGON_MODE_MAX_ENUM;
	}
}

VkCullModeFlags MusaCullToVk(CullingMode mode)
{
	switch (mode)
	{
		case CullingMode::None:
			return VK_CULL_MODE_NONE;
		case CullingMode::Back:
			return VK_CULL_MODE_BACK_BIT;
		case CullingMode::Front:
			return VK_CULL_MODE_FRONT_BIT;
		case CullingMode::FrontAndBack:
			return VK_CULL_MODE_FRONT_AND_BACK;
		default:
			return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
	}
}

VkCompareOp MusaCompareOpToVk(CompareOperation op)
{
	switch (op)
	{
		case CompareOperation::None:
			return VK_COMPARE_OP_NEVER;
		case CompareOperation::Equal:
			return VK_COMPARE_OP_EQUAL;
		case CompareOperation::NotEqual:
			return VK_COMPARE_OP_NOT_EQUAL;
		case CompareOperation::Less:
			return VK_COMPARE_OP_LESS;
		case CompareOperation::LessThanOrEqual:
			return VK_COMPARE_OP_LESS_OR_EQUAL;
		case CompareOperation::Greater:
			return VK_COMPARE_OP_GREATER;
		case CompareOperation::GreaterThanOrEqual:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case CompareOperation::Always:
			return VK_COMPARE_OP_ALWAYS;
		default:
			return VK_COMPARE_OP_MAX_ENUM;
	}
}

VkAttachmentLoadOp MusaLoadToVk(LoadOperation op)
{
	switch (op)
	{
		case LoadOperation::Load:
			return VK_ATTACHMENT_LOAD_OP_LOAD;
		case LoadOperation::Clear:
			return VK_ATTACHMENT_LOAD_OP_CLEAR;
		case LoadOperation::DontCare:
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		default:
			return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
	}
}

VkAttachmentStoreOp MusaStoreToVk(StoreOperation op)
{
	switch (op)
	{
		case StoreOperation::Store:
			return VK_ATTACHMENT_STORE_OP_STORE;
		case StoreOperation::DontCare:
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		default:
			return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
	}
}

VkBlendOp MusaBlendOpToVk(BlendOperation op)
{
	switch (op)
	{
		case BlendOperation::None:
			return VK_BLEND_OP_ADD;
		case BlendOperation::Add:
			return VK_BLEND_OP_ADD;
		case BlendOperation::Subtract:
			return VK_BLEND_OP_SUBTRACT;
		default:
			return VK_BLEND_OP_MAX_ENUM;
	}
}

VkBlendFactor MusaBlendFactorToVk(BlendFactor factor)
{
	switch (factor)
	{
		case BlendFactor::Zero:
			return VK_BLEND_FACTOR_ZERO;
		case BlendFactor::One:
			return VK_BLEND_FACTOR_ONE;
		case BlendFactor::SrcColor:
			return VK_BLEND_FACTOR_SRC_COLOR;
		case BlendFactor::OneMinusSrcColor:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case BlendFactor::DstColor:
			return VK_BLEND_FACTOR_DST_COLOR;
		case BlendFactor::OneMinusDstColor:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case BlendFactor::SrcAlpha:
			return VK_BLEND_FACTOR_SRC_ALPHA;
		case BlendFactor::OneMinusSrcAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::DstAlpha:
			return VK_BLEND_FACTOR_DST_ALPHA;
		case BlendFactor::OneMinusDstAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		case BlendFactor::ConstColor:
			return VK_BLEND_FACTOR_CONSTANT_COLOR;
		case BlendFactor::OneMinusConstColor:
			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
		case BlendFactor::ConstAlpha:
			return VK_BLEND_FACTOR_CONSTANT_ALPHA;
		case BlendFactor::OneMinusConstAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
		default:
			return VK_BLEND_FACTOR_MAX_ENUM;
	}
}

VkShaderStageFlagBits MusaStageToVkStage(ShaderStage::Type shaderStage)
{
	switch (shaderStage)
	{
		case ShaderStage::Vertex:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case ShaderStage::Fragment:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		case ShaderStage::TessalationControl:
			return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case ShaderStage::TessalationEval:
			return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		case ShaderStage::Geometry:
			return VK_SHADER_STAGE_GEOMETRY_BIT;
		case ShaderStage::Compute:
			return VK_SHADER_STAGE_COMPUTE_BIT;
		default:
			Assert(false);
			return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	}
}

VkFilter MusaFilterToVkFilter(SamplerFilter filter)
{
	switch (filter)
	{
		case SamplerFilter::Nearest:
			return VK_FILTER_NEAREST;
		case SamplerFilter::Linear:
			return VK_FILTER_LINEAR;
		default:
			return VK_FILTER_MAX_ENUM;
	}
}

VkSamplerAddressMode MusaAddressModeToVkAddressMode(SamplerAddressMode addrMode)
{
	switch (addrMode)
	{
		case SamplerAddressMode::Repeat:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case SamplerAddressMode::Clamp:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case SamplerAddressMode::Mirror:
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		default:
			return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
	}
}

VkSamplerMipmapMode MusaMipModeToVkMipMode(SamplerMipmapMode mipMode)
{
	switch (mipMode)
	{
		case SamplerMipmapMode::Nearest:
			return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		case SamplerMipmapMode::Linear:
			return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		default:
			return VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
	}
}
