// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"

class VulkanDescriptorSetLayout;
class VulkanDevice;


// TODO - This is a temporary class that should be removed later on. There needs to be a better system, will probably be managed by whatever creates pipelines
class VulkanDescriptorLayoutManager
{
public:
	VulkanDescriptorLayoutManager() = default;

	VulkanDescriptorSetLayout* CreateSetLayout();

	// TODO - This class needs to be removed so that I don't hVe to do shit like this!
	void Deinitialize();

private:
	friend class VulkanDevice;
	DynamicArray<VulkanDescriptorSetLayout*> layouts;
	VulkanDevice* logicalDevice = nullptr;
};

VulkanDescriptorLayoutManager& GetDescriptorLayoutManager();
