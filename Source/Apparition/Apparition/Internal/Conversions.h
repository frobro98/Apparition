#pragma once

#include "Apparition/RenderingDescription.h"
#include "Apparition/PipelineStateDefinitions.h"
#include "VulkanDefinitions.h"

constexpr VkImageUsageFlags ApparitionImageUsageToVk(AptnImageUsageFlags imageUsageFlags)
{
	VkImageUsageFlags vkUsageFlags = 0;
	if (imageUsageFlags & AptnImageUsageFlagBits::TransferSrc)
	{
		vkUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}
	if (imageUsageFlags & AptnImageUsageFlagBits::TransferDst)
	{
		vkUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
	if (imageUsageFlags & AptnImageUsageFlagBits::Sampled)
	{
		vkUsageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
	}
	if (imageUsageFlags & AptnImageUsageFlagBits::ColorAttachment)
	{
		vkUsageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	if (imageUsageFlags & AptnImageUsageFlagBits::DepthStencilAttachment)
	{
		vkUsageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	}

	return vkUsageFlags;
}

constexpr VkFormat ApparitionInputFormatToVk(AptnVertexInputFormat::Type type)
{
	switch (type)
	{
	case AptnVertexInputFormat::F32_1:
		return VK_FORMAT_R32_SFLOAT;
	case AptnVertexInputFormat::F32_2:
		return VK_FORMAT_R32G32_SFLOAT;
	case AptnVertexInputFormat::F32_3:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case AptnVertexInputFormat::F32_4:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case AptnVertexInputFormat::U32:
		return VK_FORMAT_R8G8B8A8_UNORM;
	default:
		Assert(false);
		return VK_FORMAT_UNDEFINED;
	}
}

constexpr VkVertexInputRate ApparitionInputRateToVk(AptnVertexInputRate::Type rate)
{
	switch (rate)
	{
	case AptnVertexInputRate::Vertex:
		return VK_VERTEX_INPUT_RATE_VERTEX;
	case AptnVertexInputRate::Instance:
		return VK_VERTEX_INPUT_RATE_INSTANCE;
	default:
		Assert(false);
		return VK_VERTEX_INPUT_RATE_MAX_ENUM;
	}
}

constexpr VkPrimitiveTopology ApparitionTopologyToVk(AptnPrimitiveTopology::Type topology)
{
	switch (topology)
	{
	case AptnPrimitiveTopology::TriangleList:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case AptnPrimitiveTopology::TriangleStrip:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	case AptnPrimitiveTopology::TriangleFan:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	case AptnPrimitiveTopology::LineList:
		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	case AptnPrimitiveTopology::LineStrip:
		return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	case AptnPrimitiveTopology::PointList:
		return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	default:
		Assert(false);
		return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
	}
}

constexpr VkPolygonMode ApparitionFillToVk(AptnFillMode::Type mode)
{
	switch (mode)
	{
	case AptnFillMode::Full:
		return VK_POLYGON_MODE_FILL;
	case AptnFillMode::Wireframe:
		return VK_POLYGON_MODE_LINE;
	case AptnFillMode::Point:
		return VK_POLYGON_MODE_POINT;
	default:
		Assert(false);
		return VK_POLYGON_MODE_MAX_ENUM;
	}
}

constexpr VkCullModeFlags ApparitionCullToVk(AptnCullMode::Type mode)
{
	switch (mode)
	{
	case AptnCullMode::None:
		return VK_CULL_MODE_NONE;
	case AptnCullMode::Back:
		return VK_CULL_MODE_BACK_BIT;
	case AptnCullMode::Front:
		return VK_CULL_MODE_FRONT_BIT;
	case AptnCullMode::FrontAndBack:
		return VK_CULL_MODE_FRONT_AND_BACK;
	default:
		Assert(false);
		return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
	}
}

constexpr VkFrontFace ApparitionFrontFaceToVk(AptnFrontFace::Type frontFace)
{
	switch (frontFace)
	{
	case AptnFrontFace::Clockwise:
		return VK_FRONT_FACE_CLOCKWISE;
	case AptnFrontFace::CounterClockwise:
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	default:
		Assert(false);
		return VK_FRONT_FACE_MAX_ENUM;
	}
}

constexpr VkCompareOp ApparitionCompareOpToVk(AptnCompareOperation::Type op)
{
	switch (op)
	{
	case AptnCompareOperation::None:
		return VK_COMPARE_OP_NEVER;
	case AptnCompareOperation::Equal:
		return VK_COMPARE_OP_EQUAL;
	case AptnCompareOperation::NotEqual:
		return VK_COMPARE_OP_NOT_EQUAL;
	case AptnCompareOperation::Less:
		return VK_COMPARE_OP_LESS;
	case AptnCompareOperation::LessThanOrEqual:
		return VK_COMPARE_OP_LESS_OR_EQUAL;
	case AptnCompareOperation::Greater:
		return VK_COMPARE_OP_GREATER;
	case AptnCompareOperation::GreaterThanOrEqual:
		return VK_COMPARE_OP_GREATER_OR_EQUAL;
	case AptnCompareOperation::Always:
		return VK_COMPARE_OP_ALWAYS;
	default:
		Assert(false);
		return VK_COMPARE_OP_MAX_ENUM;
	}
}

constexpr VkAttachmentLoadOp ApparitionLoadToVkLoad(AptnLoadOperation::Type loadOp)
{
	switch (loadOp)
	{
	case AptnLoadOperation::Load:
		return VK_ATTACHMENT_LOAD_OP_LOAD;
	case AptnLoadOperation::Clear:
		return VK_ATTACHMENT_LOAD_OP_CLEAR;
	case AptnLoadOperation::DontCare:
		return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	default:
		Assert(false);
		return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
	}
}

constexpr VkAttachmentStoreOp ApparitionStoreToVkStore(AptnStoreOperation::Type storeOp)
{
	switch (storeOp)
	{
	case AptnStoreOperation::Store:
		return VK_ATTACHMENT_STORE_OP_STORE;
	case AptnStoreOperation::DontCare:
		return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	default:
		Assert(false);
		return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
	}
}

constexpr VkColorComponentFlags ApparitionColorWriteMaskToVk(AptnColorComponentFlags colorMaskFlags)
{
	VkColorComponentFlags vkColorMask = 0;
	if (colorMaskFlags & AptnColorComponentFlagBits::Red)
	{
		vkColorMask |= VK_COLOR_COMPONENT_R_BIT;
	}
	if (colorMaskFlags & AptnColorComponentFlagBits::Green)
	{
		vkColorMask |= VK_COLOR_COMPONENT_G_BIT;
	}
	if (colorMaskFlags & AptnColorComponentFlagBits::Blue)
	{
		vkColorMask |= VK_COLOR_COMPONENT_B_BIT;
	}
	if (colorMaskFlags & AptnColorComponentFlagBits::Alpha)
	{
		vkColorMask |= VK_COLOR_COMPONENT_A_BIT;
	}

	return vkColorMask;
}

constexpr VkBlendOp ApparitionBlendOpToVk(AptnBlendOperation::Type op)
{
	switch (op)
	{
	case AptnBlendOperation::None:
		return VK_BLEND_OP_ADD;
	case AptnBlendOperation::Add:
		return VK_BLEND_OP_ADD;
	case AptnBlendOperation::Subtract:
		return VK_BLEND_OP_SUBTRACT;
	default:
		return VK_BLEND_OP_MAX_ENUM;
	}
}

constexpr VkBlendFactor ApparitionBlendFactorToVk(AptnBlendFactor::Type factor)
{
	switch (factor)
	{
	case AptnBlendFactor::Zero:
		return VK_BLEND_FACTOR_ZERO;
	case AptnBlendFactor::One:
		return VK_BLEND_FACTOR_ONE;
	case AptnBlendFactor::SrcColor:
		return VK_BLEND_FACTOR_SRC_COLOR;
	case AptnBlendFactor::OneMinusSrcColor:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	case AptnBlendFactor::DstColor:
		return VK_BLEND_FACTOR_DST_COLOR;
	case AptnBlendFactor::OneMinusDstColor:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
	case AptnBlendFactor::SrcAlpha:
		return VK_BLEND_FACTOR_SRC_ALPHA;
	case AptnBlendFactor::OneMinusSrcAlpha:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	case AptnBlendFactor::DstAlpha:
		return VK_BLEND_FACTOR_DST_ALPHA;
	case AptnBlendFactor::OneMinusDstAlpha:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
	case AptnBlendFactor::ConstColor:
		return VK_BLEND_FACTOR_CONSTANT_COLOR;
	case AptnBlendFactor::OneMinusConstColor:
		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
	case AptnBlendFactor::ConstAlpha:
		return VK_BLEND_FACTOR_CONSTANT_ALPHA;
	case AptnBlendFactor::OneMinusConstAlpha:
		return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
	default:
		return VK_BLEND_FACTOR_MAX_ENUM;
	}
}

constexpr VkDescriptorType ApparitionDescriptorTypeToVk(AptnDescriptor::Type descriptorType)
{
	switch (descriptorType)
	{
	case AptnDescriptor::Sampler:
		return VK_DESCRIPTOR_TYPE_SAMPLER;
	case AptnDescriptor::CombinedImageSampler:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case AptnDescriptor::SampledImage:
		return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	case AptnDescriptor::StorageImage:
		return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	case AptnDescriptor::UniformTexelBuffer:
		return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
	case AptnDescriptor::StorageTexelBuffer:
		return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
	case AptnDescriptor::UniformBuffer:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case AptnDescriptor::StorageBuffer:
		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	case AptnDescriptor::UniformBufferDynamic:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	case AptnDescriptor::StorageBufferDynamic:
		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
	case AptnDescriptor::InputAttachment:
		return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	case AptnDescriptor::Count:
	case AptnDescriptor::Max:
	default:
		Assert(false);
	}

	return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

constexpr VkShaderStageFlags ApparitionShaderFlagsToVk(AptnShaderStageFlags shaderFlags)
{
	VkShaderStageFlags flags = 0;
	if (shaderFlags & AptnShaderStageFlagBits::Vertex)
	{
		flags |= VK_SHADER_STAGE_VERTEX_BIT;
	}

	if (shaderFlags & AptnShaderStageFlagBits::Fragment)
	{
		flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	return flags;
}

constexpr VkFilter ApparitionFilterToVk(AptnSamplerFilter::Type filter)
{
	switch (filter)
	{
	case AptnSamplerFilter::Nearest:
		return VK_FILTER_NEAREST;
	case AptnSamplerFilter::Linear:
		return VK_FILTER_LINEAR;
	default:
		Assert(false);
	}

	return VK_FILTER_MAX_ENUM;
}

constexpr VkSamplerAddressMode ApparitionAddressModeToVk(AptnSamplerAddressMode::Type addrMode)
{
	switch (addrMode)
	{
	case AptnSamplerAddressMode::Repeat:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	case AptnSamplerAddressMode::Clamp:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case AptnSamplerAddressMode::Mirror:
		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	default:
		Assert(false);
	}

	return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
}

constexpr VkSamplerMipmapMode ApparitionMipModeToVk(AptnSamplerMipmapMode::Type mipMode)
{
	switch (mipMode)
	{
	case AptnSamplerMipmapMode::Nearest:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	case AptnSamplerMipmapMode::Linear:
		return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	default:
		Assert(false);
	}

	return VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
}
