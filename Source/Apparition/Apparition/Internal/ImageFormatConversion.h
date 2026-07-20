#pragma once

#include "Apparition/ImageDescription.h"
#include "Debugging/Assertion.hpp"
#include "VulkanDefinitions.h"

// Sets up quick mapping between the formats themselves instead of doing a switch
void InitializeFormatMapping();

Apparition::ImageFormat::Type VkFormatToApparition(VkFormat format);
VkFormat ApparitionFormatToVk(Apparition::ImageFormat::Type imageFormat);

constexpr VkImageAspectFlags ApparitionImageViewAspectToVkAspectFlags(Apparition::ImageAspect::Type aspect)
{
	switch (aspect)
	{
	case Apparition::ImageAspect::Color:
		return VK_IMAGE_ASPECT_COLOR_BIT;
	case Apparition::ImageAspect::Depth:
		return VK_IMAGE_ASPECT_DEPTH_BIT;
	case Apparition::ImageAspect::Stencil:
		return VK_IMAGE_ASPECT_STENCIL_BIT;
	default:
		return VK_IMAGE_ASPECT_NONE;
	}
}

constexpr VkImageLayout ApparitionImageAccessToVkLayout(Apparition::ImageAccess::Type imgState)
{
	switch (imgState)
	{
	case Apparition::ImageAccess::Present:
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	case Apparition::ImageAccess::TransferDst:
		return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	case Apparition::ImageAccess::ColorWrite:
	case Apparition::ImageAccess::DepthStencilWrite:
		return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;

	case Apparition::ImageAccess::ColorRead:
	case Apparition::ImageAccess::DepthStencilRead:
		return VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;

	case Apparition::ImageAccess::Undefined:
	default:
		return VK_IMAGE_LAYOUT_UNDEFINED;
	}
}

constexpr VkAccessFlags2 ApparitionImageAccessToAccessMask(Apparition::ImageAccess::Type imgAccess)
{
	switch (imgAccess)
	{
	case Apparition::ImageAccess::Present:
		return 0;
	case Apparition::ImageAccess::TransferSrc:
		return VK_ACCESS_2_TRANSFER_READ_BIT;
	case Apparition::ImageAccess::TransferDst:
		return VK_ACCESS_2_TRANSFER_WRITE_BIT;
	case Apparition::ImageAccess::ColorWrite:
		return VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
	case Apparition::ImageAccess::ColorRead:
		return VK_ACCESS_2_SHADER_READ_BIT;
	case Apparition::ImageAccess::DepthStencilWrite:
		return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	case Apparition::ImageAccess::DepthStencilRead:
		return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	case Apparition::ImageAccess::Undefined:
	default:
		return VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
	}
}

constexpr VkPipelineStageFlags2 ApparitionImageAccessToPipelineStage(Apparition::ImageAccess::Type imgAccess)
{
	switch (imgAccess)
	{
	case Apparition::ImageAccess::Present:
		return VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
	case Apparition::ImageAccess::TransferDst:
		return VK_PIPELINE_STAGE_2_TRANSFER_BIT;
	case Apparition::ImageAccess::ColorWrite:
		return VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	case Apparition::ImageAccess::ColorRead:
		return VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
	case Apparition::ImageAccess::DepthStencilWrite:
		return VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
	case Apparition::ImageAccess::DepthStencilRead:
		return VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
	case Apparition::ImageAccess::Undefined:
	default:
		return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	}
}
