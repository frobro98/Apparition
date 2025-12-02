#pragma once

#include "Apparition/ImageDescription.h"
#include "VulkanDefinitions.h"

// Sets up quick mapping between the formats themselves instead of doing a switch
void InitializeFormatMapping();

Apparition::ImageFormat::Type VkFormatToApparitionFormat(VkFormat format);
VkFormat ApparitionFormatToVkFormat(Apparition::ImageFormat::Type imageFormat);
