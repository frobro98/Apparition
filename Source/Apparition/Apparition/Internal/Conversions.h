#pragma once

#include "Apparition/RenderingDescription.h"
#include "Apparition/PipelineStateDefinitions.h"
#include "VulkanDefinitions.h"

constexpr VkImageUsageFlags ApparitionImageUsageToVk(Apparition::ImageUsageFlags imageUsageFlags)
{
	VkImageUsageFlags vkUsageFlags = 0;
	if (imageUsageFlags & Apparition::ImageUsageFlagBits::TransferSrc)
	{
		vkUsageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}
	if (imageUsageFlags & Apparition::ImageUsageFlagBits::TransferDst)
	{
		vkUsageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
	if (imageUsageFlags & Apparition::ImageUsageFlagBits::Sampled)
	{
		vkUsageFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
	}
	if (imageUsageFlags & Apparition::ImageUsageFlagBits::ColorAttachment)
	{
		vkUsageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	if (imageUsageFlags & Apparition::ImageUsageFlagBits::DepthStencilAttachment)
	{
		vkUsageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	}

	return vkUsageFlags;
}

constexpr VkFormat ApparitionInputFormatToVk(Apparition::VertexInputFormat::Type type)
{
	using namespace Apparition;
	switch (type)
	{
	case VertexInputFormat::F32_1:
		return VK_FORMAT_R32_SFLOAT;
	case VertexInputFormat::F32_2:
		return VK_FORMAT_R32G32_SFLOAT;
	case VertexInputFormat::F32_3:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case VertexInputFormat::F32_4:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	case VertexInputFormat::U32:
		return VK_FORMAT_R8G8B8A8_UNORM;
	default:
		Assert(false);
		return VK_FORMAT_UNDEFINED;
	}
}

constexpr VkVertexInputRate ApparitionInputRateToVk(Apparition::VertexInputRate::Type rate)
{
	using namespace Apparition;
	switch (rate)
	{
	case VertexInputRate::Vertex:
		return VK_VERTEX_INPUT_RATE_VERTEX;
	case VertexInputRate::Instance:
		return VK_VERTEX_INPUT_RATE_INSTANCE;
	default:
		Assert(false);
		return VK_VERTEX_INPUT_RATE_MAX_ENUM;
	}
}

constexpr VkPrimitiveTopology ApparitionTopologyToVk(Apparition::PrimitiveTopology::Type topology)
{
	using namespace Apparition;
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
		Assert(false);
		return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
	}
}

constexpr VkPolygonMode ApparitionFillToVk(Apparition::FillMode::Type mode)
{
	using namespace Apparition;
	switch (mode)
	{
	case FillMode::Full:
		return VK_POLYGON_MODE_FILL;
	case FillMode::Wireframe:
		return VK_POLYGON_MODE_LINE;
	case FillMode::Point:
		return VK_POLYGON_MODE_POINT;
	default:
		Assert(false);
		return VK_POLYGON_MODE_MAX_ENUM;
	}
}

constexpr VkCullModeFlags ApparitionCullToVk(Apparition::CullMode::Type mode)
{
	using namespace Apparition;
	switch (mode)
	{
	case CullMode::None:
		return VK_CULL_MODE_NONE;
	case CullMode::Back:
		return VK_CULL_MODE_BACK_BIT;
	case CullMode::Front:
		return VK_CULL_MODE_FRONT_BIT;
	case CullMode::FrontAndBack:
		return VK_CULL_MODE_FRONT_AND_BACK;
	default:
		Assert(false);
		return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
	}
}

constexpr VkFrontFace ApparitionFrontFaceToVk(Apparition::FrontFace::Type frontFace)
{
	using namespace Apparition;
	switch (frontFace)
	{
	case FrontFace::Clockwise:
		return VK_FRONT_FACE_CLOCKWISE;
	case FrontFace::CounterClockwise:
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	default:
		Assert(false);
		return VK_FRONT_FACE_MAX_ENUM;
	}
}

constexpr VkCompareOp ApparitionCompareOpToVk(CompareOperation::Type op)
{
	using namespace Apparition;
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
		Assert(false);
		return VK_COMPARE_OP_MAX_ENUM;
	}
}

constexpr VkAttachmentLoadOp ApparitionLoadToVkLoad(Apparition::LoadOperation::Type loadOp)
{
	using namespace Apparition;

	switch (loadOp)
	{
	case LoadOperation::Load:
		return VK_ATTACHMENT_LOAD_OP_LOAD;
	case LoadOperation::Clear:
		return VK_ATTACHMENT_LOAD_OP_CLEAR;
	case LoadOperation::DontCare:
		return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	default:
		Assert(false);
		return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
	}
}

constexpr VkAttachmentStoreOp ApparitionStoreToVkStore(Apparition::StoreOperation::Type storeOp)
{
	using namespace Apparition;

	switch (storeOp)
	{
	case StoreOperation::Store:
		return VK_ATTACHMENT_STORE_OP_STORE;
	case StoreOperation::DontCare:
		return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	default:
		Assert(false);
		return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
	}
}

constexpr VkColorComponentFlags ApparitionColorWriteMaskToVk(Apparition::ColorComponentFlags colorMaskFlags)
{
	using namespace Apparition;

	VkColorComponentFlags vkColorMask = 0;
	if (colorMaskFlags & ColorComponentFlagBits::Red)
	{
		vkColorMask |= VK_COLOR_COMPONENT_R_BIT;
	}
	if (colorMaskFlags & ColorComponentFlagBits::Green)
	{
		vkColorMask |= VK_COLOR_COMPONENT_G_BIT;
	}
	if (colorMaskFlags & ColorComponentFlagBits::Blue)
	{
		vkColorMask |= VK_COLOR_COMPONENT_B_BIT;
	}
	if (colorMaskFlags & ColorComponentFlagBits::Alpha)
	{
		vkColorMask |= VK_COLOR_COMPONENT_A_BIT;
	}

	return vkColorMask;
}

constexpr VkBlendOp ApparitionBlendOpToVk(Apparition::BlendOperation::Type op)
{
	using namespace Apparition;
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

constexpr VkBlendFactor ApparitionBlendFactorToVk(Apparition::BlendFactor::Type factor)
{
	using namespace Apparition;
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

constexpr VkDescriptorType ApparitionDescriptorTypeToVk(Apparition::Descriptor::Type descriptorType)
{
	using namespace Apparition;
	switch (descriptorType)
	{
	case Apparition::Descriptor::Sampler:
		return VK_DESCRIPTOR_TYPE_SAMPLER;
	case Apparition::Descriptor::CombinedImageSampler:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case Apparition::Descriptor::SampledImage:
		return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	case Apparition::Descriptor::StorageImage:
		return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	case Apparition::Descriptor::UniformTexelBuffer:
		return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
	case Apparition::Descriptor::StorageTexelBuffer:
		return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
	case Apparition::Descriptor::UniformBuffer:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	case Apparition::Descriptor::StorageBuffer:
		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	case Apparition::Descriptor::UniformBufferDynamic:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	case Apparition::Descriptor::StorageBufferDynamic:
		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
	case Apparition::Descriptor::InputAttachment:
		return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	case Apparition::Descriptor::Count:
	case Apparition::Descriptor::Max:
	default:
		Assert(false);
	}

	return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

constexpr VkFilter ApparitionFilterToVk(Apparition::SamplerFilter::Type filter)
{
	using namespace Apparition;

	switch (filter)
	{
	case SamplerFilter::Nearest:
		return VK_FILTER_NEAREST;
	case SamplerFilter::Linear:
		return VK_FILTER_LINEAR;
	default:
		Assert(false);
	}

	return VK_FILTER_MAX_ENUM;
}

constexpr VkSamplerAddressMode ApparitionAddressModeToVk(Apparition::SamplerAddressMode::Type addrMode)
{
	using namespace Apparition;

	switch (addrMode)
	{
	case SamplerAddressMode::Repeat:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	case SamplerAddressMode::Clamp:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case SamplerAddressMode::Mirror:
		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	default:
		Assert(false);
	}

	return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
}

constexpr VkSamplerMipmapMode ApparitionMipModeToVk(Apparition::SamplerMipmapMode::Type mipMode)
{
	using namespace Apparition;

	switch (mipMode)
	{
	case SamplerMipmapMode::Nearest:
		return VK_SAMPLER_MIPMAP_MODE_NEAREST;
	case SamplerMipmapMode::Linear:
		return VK_SAMPLER_MIPMAP_MODE_LINEAR;
	default:
		Assert(false);
	}

	return VK_SAMPLER_MIPMAP_MODE_MAX_ENUM;
}
