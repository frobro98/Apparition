#pragma once

#include "Apparition/ImageDescription.h"
#include "Debugging/Assertion.hpp"
#include "VulkanDefinitions.h"

// Sets up quick mapping between the formats themselves instead of doing a switch
void InitializeFormatMapping();

AptnImageFormat::Type VkFormatToApparition(VkFormat format);
VkFormat ApparitionFormatToVk(AptnImageFormat::Type imageFormat);

constexpr VkImageAspectFlags ApparitionImageViewAspectToVkAspectFlags(AptnImageAspect::Type aspect)
{
	switch (aspect)
	{
	case AptnImageAspect::Color:
		return VK_IMAGE_ASPECT_COLOR_BIT;
	case AptnImageAspect::Depth:
		return VK_IMAGE_ASPECT_DEPTH_BIT;
	case AptnImageAspect::Stencil:
		return VK_IMAGE_ASPECT_STENCIL_BIT;
	case AptnImageAspect::DepthStencil:
		return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	default:
		return VK_IMAGE_ASPECT_NONE;
	}
}

constexpr VkImageLayout ApparitionImageAccessToVkLayout(AptnImageAccess::Type imgState)
{
	switch (imgState)
	{
	case AptnImageAccess::Present:
		return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	case AptnImageAccess::TransferDst:
		return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

	case AptnImageAccess::ColorWrite:
	case AptnImageAccess::DepthStencilWrite:
		return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;

	case AptnImageAccess::ColorRead:
	case AptnImageAccess::DepthStencilRead:
		return VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;

	case AptnImageAccess::Undefined:
	default:
		return VK_IMAGE_LAYOUT_UNDEFINED;
	}
}

constexpr VkAccessFlags2 ApparitionImageAccessToAccessMask(AptnImageAccess::Type imgAccess)
{
	switch (imgAccess)
	{
	case AptnImageAccess::Present:
		return 0;
	case AptnImageAccess::TransferSrc:
		return VK_ACCESS_2_TRANSFER_READ_BIT;
	case AptnImageAccess::TransferDst:
		return VK_ACCESS_2_TRANSFER_WRITE_BIT;
	case AptnImageAccess::ColorWrite:
		return VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
	case AptnImageAccess::ColorRead:
		return VK_ACCESS_2_SHADER_READ_BIT;
	case AptnImageAccess::DepthStencilWrite:
		return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	case AptnImageAccess::DepthStencilRead:
		return VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
	case AptnImageAccess::Undefined:
	default:
		return VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
	}
}

constexpr VkPipelineStageFlags2 ApparitionImageAccessToPipelineStage(AptnImageAccess::Type imgAccess)
{
	switch (imgAccess)
	{
	case AptnImageAccess::Present:
		return VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
	case AptnImageAccess::TransferDst:
		return VK_PIPELINE_STAGE_2_TRANSFER_BIT;
	case AptnImageAccess::ColorWrite:
		return VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	case AptnImageAccess::ColorRead:
		return VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
	case AptnImageAccess::DepthStencilWrite:
		return VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
	case AptnImageAccess::DepthStencilRead:
		return VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
	case AptnImageAccess::Undefined:
	default:
		return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	}
}
