#pragma once

#include "Apparition/RenderingDescription.h"
#include "VulkanDefinitions.h"

constexpr VkAttachmentLoadOp ApparitionLoadToVkLoad(Apparition::LoadOperation loadOp)
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
		return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
	}
}

constexpr VkAttachmentStoreOp ApparitionStoreToVkStore(Apparition::StoreOperation storeOp)
{
	using namespace Apparition;

	switch (storeOp)
	{
	case StoreOperation::Store:
		return VK_ATTACHMENT_STORE_OP_STORE;
	case StoreOperation::DontCare:
		return VK_ATTACHMENT_STORE_OP_DONT_CARE;
	default:
		return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
	}
}

