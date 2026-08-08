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
	AptnDevice                 device;
	AptnImage                  image;
	AptnImageAccess::Type      imageAccess;
	AptnImageView              view;
	uint32_t                           width, height;
	uint32_t                           mipLevels;
	uint32_t                           layerCount;
	AptnImageDescriptorInfo	   descriptor;
	AptnSampler                sampler;
	AptnImageFormat::Type      format;

	void      updateDescriptor();
	void      destroy();
	ktxResult loadKTXFile(std::string filename, ktxTexture **target);
};

class Texture2D : public Texture
{
  public:
	void loadFromFile(
	    std::string                    filename,
	    AptnImageFormat::Type  format,
		AptnDevice             device,
	    AptnQueue              copyQueue,
		AptnImageUsageFlags    imageUsageFlags = AptnImageUsageFlagBits::Sampled,
		AptnImageAccess::Type  imageAccess = AptnImageAccess::ColorRead);
	void fromBuffer(
	    void *                          buffer,
	    VkDeviceSize                    bufferSize,
	    AptnImageFormat::Type   format,
	    uint32_t                        texWidth,
	    uint32_t                        texHeight,
		AptnDevice              device,
	    AptnQueue               copyQueue,
	    AptnSamplerFilter::Type filter = AptnSamplerFilter::Linear,
		AptnImageUsageFlags     imageUsageFlags = AptnImageUsageFlagBits::Sampled,
		AptnImageAccess::Type   imageAccess = AptnImageAccess::ColorRead);
};

class Texture2DArray : public Texture
{
  public:
	/*
	void loadFromFile(
	    std::string                    filename,
		AptnImageFormat::Type  format,
		AptnDevice             device,
		AptnQueue              copyQueue,
		AptnImageUsageFlags    imageUsageFlags = AptnImageUsageFlagBits::Sampled,
		AptnImageAccess::Type  imageAccess = AptnImageAccess::ColorRead);
	//*/
};

class TextureCubeMap : public Texture
{
  public:
	/*
	void loadFromFile(
	    std::string                    filename,
		AptnImageFormat::Type  format,
		AptnDevice             device,
		AptnQueue              copyQueue,
		AptnImageUsageFlags    imageUsageFlags = AptnImageUsageFlagBits::Sampled,
		AptnImageAccess::Type  imageAccess = AptnImageAccess::ColorRead);
	//*/
};
}        // namespace vks
