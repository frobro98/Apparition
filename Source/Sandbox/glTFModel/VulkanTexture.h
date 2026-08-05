/*
* Vulkan texture loader for KTX files
*
* Copyright (C) 2016-2025 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license(MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>

#include "vulkan/vulkan.h"

#include <ktx.h>
#include <ktxvulkan.h>

#include "Apparition/Buffer.h"
#include "Apparition/DescriptorSet.h"
#include "Apparition/Device.h"
#include "Apparition/Image.h"
#include "Apparition/Queue.h"

#if defined(__ANDROID__)
#	include <android/asset_manager.h>
#endif

namespace vks
{
class Texture
{
  public:
	Apparition::Device                 device;
	Apparition::Image                  image;
	Apparition::ImageAccess::Type      imageAccess;
	Apparition::ImageView              view;
	uint32_t                           width, height;
	uint32_t                           mipLevels;
	uint32_t                           layerCount;
	Apparition::ImageDescriptorInfo	   descriptor;
	Apparition::Sampler                sampler;
	Apparition::ImageFormat::Type      format;

	void      updateDescriptor();
	void      destroy();
	ktxResult loadKTXFile(std::string filename, ktxTexture **target);
};

class Texture2D : public Texture
{
  public:
	void loadFromFile(
	    std::string                    filename,
	    Apparition::ImageFormat::Type  format,
		Apparition::Device             device,
	    Apparition::Queue              copyQueue,
		Apparition::ImageUsageFlags    imageUsageFlags = Apparition::ImageUsageFlagBits::Sampled,
		Apparition::ImageAccess::Type  imageAccess = Apparition::ImageAccess::ColorRead);
	void fromBuffer(
	    void *                          buffer,
	    VkDeviceSize                    bufferSize,
	    Apparition::ImageFormat::Type   format,
	    uint32_t                        texWidth,
	    uint32_t                        texHeight,
		Apparition::Device              device,
	    Apparition::Queue               copyQueue,
	    Apparition::SamplerFilter::Type filter = Apparition::SamplerFilter::Linear,
		Apparition::ImageUsageFlags     imageUsageFlags = Apparition::ImageUsageFlagBits::Sampled,
		Apparition::ImageAccess::Type   imageAccess = Apparition::ImageAccess::ColorRead);
};

class Texture2DArray : public Texture
{
  public:
	/*
	void loadFromFile(
	    std::string                    filename,
		Apparition::ImageFormat::Type  format,
		Apparition::Device             device,
		Apparition::Queue              copyQueue,
		Apparition::ImageUsageFlags    imageUsageFlags = Apparition::ImageUsageFlagBits::Sampled,
		Apparition::ImageAccess::Type  imageAccess = Apparition::ImageAccess::ColorRead);
	//*/
};

class TextureCubeMap : public Texture
{
  public:
	/*
	void loadFromFile(
	    std::string                    filename,
		Apparition::ImageFormat::Type  format,
		Apparition::Device             device,
		Apparition::Queue              copyQueue,
		Apparition::ImageUsageFlags    imageUsageFlags = Apparition::ImageUsageFlagBits::Sampled,
		Apparition::ImageAccess::Type  imageAccess = Apparition::ImageAccess::ColorRead);
	//*/
};
}        // namespace vks
