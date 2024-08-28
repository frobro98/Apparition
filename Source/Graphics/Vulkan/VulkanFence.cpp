// Copyright 2020, Nathan Blane

#include "VulkanFence.hpp"
#include "VulkanDevice.h"

VulkanFence::VulkanFence(const VulkanDevice& device, VulkanFenceManager& manager)
	: fenceManager(&manager),
	logicalDevice(&device)
{
	VkFenceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	NOT_USED VkResult result = vkCreateFence(logicalDevice->GetNativeHandle(), &createInfo, nullptr, &fenceHandle);
	CHECK_VK(result);
}

VulkanFence::~VulkanFence()
{
	vkDestroyFence(logicalDevice->GetNativeHandle(), fenceHandle, nullptr);
}

FenceState VulkanFence::CheckFenceStatus()
{
	if (state != FenceState::Signaled)
	{
		VkResult result = vkGetFenceStatus(logicalDevice->GetNativeHandle(), fenceHandle);
		switch (result)
		{
			case VK_SUCCESS:
			{
				state = FenceState::Signaled;
			}break;
			case VK_NOT_READY:
			{
			}break;
			default:
				Assert(false);
		}
	}
	return state;
}

void VulkanFence::ResetFence(bool forceReset)
{
	if (forceReset || state == FenceState::Signaled)
	{
		vkResetFences(logicalDevice->GetNativeHandle(), 1, &fenceHandle);
		++fenceSignaledCount;
		state = FenceState::NotSignaled;
	}
}

bool VulkanFence::WaitFor(u64 nanoSeconds)
{
	bool signaled = true;
	if (state != FenceState::Signaled)
	{
		VkResult result = vkWaitForFences(logicalDevice->GetNativeHandle(), 1, &fenceHandle, true, nanoSeconds);
		switch (result)
		{
			case VK_SUCCESS:
			{
				state = FenceState::Signaled;
			}break;
			case VK_TIMEOUT:
			{
				signaled = false;
			}break;
			default:
				Assert(false);
		}
	}
	return signaled;
}

bool VulkanFence::IsSignaled()
{
	return state == FenceState::Signaled;
}

//////////////////////////////////////////////////////////////////////////

VulkanFenceManager::VulkanFenceManager(const VulkanDevice& device)
	: logicalDevice(&device)
{
}

VulkanFenceManager::~VulkanFenceManager()
{
	for (auto fence : fences)
	{
		delete fence;
	}

	fences.Clear();
}

VulkanFence* VulkanFenceManager::CreateFence()
{
	VulkanFence* fence = new VulkanFence(*logicalDevice, *this);
	fences.Add(fence);
	return fence;
}

void VulkanFenceManager::DestroyFence(const VulkanFence& fence)
{
	VulkanFence* found = nullptr;
	u32 index = 0;
	for (u32 i = 0; i < fences.Size(); ++i)
	{
		if (fence.GetNativeHandle() == fences[i]->GetNativeHandle())
		{
			found = fences[i];
			index = i;
			break;
		}
	}

	Assert(found != nullptr);
	delete found;
	fences.Remove(index);
}
