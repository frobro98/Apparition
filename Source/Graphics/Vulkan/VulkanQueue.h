// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanDefinitions.h"

class VulkanDevice;

class VulkanQueue
{
public:
	enum class Type
	{
		Graphics,
		Present,
		Copy,
		Compute
	};

public:
	VulkanQueue(VulkanDevice* device);

	void Initialize(u32 familyIndex, Type type);

	inline u32 GetFamilyIndex() const { return queueFamilyIndex; }
	inline VkQueue GetNativeHandle() const { return queue; }
private:
	VkQueue queue = VK_NULL_HANDLE;
	VulkanDevice* logicalDevice = nullptr;
	u32 queueFamilyIndex = 0;
	Type queueType = Type::Graphics;
};
