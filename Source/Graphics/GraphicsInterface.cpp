// Copyright 2020, Nathan Blane

#include "Vulkan/VulkanGraphicsInterface.hpp"

GraphicsInterface& GetGraphicsInterface()
{
	static VulkanGraphicsInterface graphicsInterface;
	return graphicsInterface;
}
