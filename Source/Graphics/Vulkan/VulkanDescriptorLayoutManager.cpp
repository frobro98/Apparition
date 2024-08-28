// Copyright 2020, Nathan Blane

#include "VulkanDescriptorLayoutManager.h"
#include "VulkanDescriptorSet.h"

VulkanDescriptorSetLayout* VulkanDescriptorLayoutManager::CreateSetLayout()
{
	VulkanDescriptorSetLayout* layout = new VulkanDescriptorSetLayout(*logicalDevice);
	layouts.Add(layout);
	return layout;
}

void VulkanDescriptorLayoutManager::Deinitialize()
{
	for (u32 i = 0; i < layouts.Size(); ++i)
	{
		delete layouts[i];
	}
}

VulkanDescriptorLayoutManager& GetDescriptorLayoutManager()
{
	static VulkanDescriptorLayoutManager manager;
	return manager;
}
