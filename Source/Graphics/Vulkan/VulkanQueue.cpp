// Copyright 2020, Nathan Blane

#include "VulkanQueue.h"
#include "VulkanDevice.h"

VulkanQueue::VulkanQueue(VulkanDevice * device)
	: logicalDevice(device)
{
}

void VulkanQueue::Initialize(u32 familyIndex, Type type)
{
	queueFamilyIndex = familyIndex;
	queueType = type;
	vkGetDeviceQueue(logicalDevice->GetNativeHandle(), queueFamilyIndex, 0, &queue);
}
