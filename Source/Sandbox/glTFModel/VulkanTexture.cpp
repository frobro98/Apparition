/*
* Vulkan texture loader for KTX files
*
* Copyright (C) 2016-2025 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license(MIT) (http://opensource.org/licenses/MIT)
*/

#include "VulkanTexture.h"

#include "Apparition/CommandBufferCommands.h"

namespace vks
{
	void Texture::updateDescriptor()
	{
		descriptor.sampler = sampler;
		descriptor.imageView = view;
		descriptor.access = imageAccess;
	}

	void Texture::destroy()
	{
		Apparition::DestroyImageView(view);
		Apparition::DestroyImage(image);
		if (IsValid(sampler))
		{
			Apparition::DestroySampler(sampler);
		}
	}

	ktxResult Texture::loadKTXFile(std::string filename, ktxTexture **target)
	{
		ktxResult result = KTX_SUCCESS;
#if defined(__ANDROID__)
		AAsset* asset = AAssetManager_open(androidApp->activity->assetManager, filename.c_str(), AASSET_MODE_STREAMING);
		if (!asset) {
			vks::tools::exitFatal("Could not load texture from " + filename + "\n\nMake sure the assets submodule has been checked out and is up-to-date.", -1);
		}
		size_t size = AAsset_getLength(asset);
		assert(size > 0);
		ktx_uint8_t *textureData = new ktx_uint8_t[size];
		AAsset_read(asset, textureData, size);
		AAsset_close(asset);
		result = ktxTexture_CreateFromMemory(textureData, size, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, target);
		delete[] textureData;
#else
		result = ktxTexture_CreateFromNamedFile(filename.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, target);			
#endif		
		return result;
	}

	/**
	* Load a 2D texture including all mip levels
	*
	* @param filename File to load (supports .ktx)
	* @param format Vulkan format of the image data stored in the file
	* @param device Vulkan device to create the texture on
	* @param copyQueue Queue used for the texture staging copy commands (must support transfer)
	* @param (Optional) imageUsageFlags Usage flags for the texture's image (defaults to VK_IMAGE_USAGE_SAMPLED_BIT)
	* @param (Optional) imageLayout Usage layout for the texture (defaults VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	*
	*/
	void Texture2D::loadFromFile(std::string filename, AptnImageFormat format, AptnDevice device, AptnQueue copyQueue, AptnImageUsageFlags imageUsageFlags, AptnImageAccess imageAccess)
	{
		ktxTexture* ktxTexture;
		ktxResult result = loadKTXFile(filename, &ktxTexture);
		Assert(result == KTX_SUCCESS);

		this->device = device;
		width = ktxTexture->baseWidth;
		height = ktxTexture->baseHeight;
		mipLevels = ktxTexture->numLevels;
		this->format = format;

		ktx_uint8_t *ktxTextureData = ktxTexture_GetData(ktxTexture);
		ktx_size_t ktxTextureSize = ktxTexture_GetSize(ktxTexture);

		// Use a separate command buffer for texture loading
		AptnCommandPoolCreationParams poolParams
		{
			.queueIndex = Apparition::GetQueueIndex(copyQueue),
			.canResetCommandBuffers = false
		};
		AptnCommandPool cmdPool = Apparition::CreateCommandPool(device, poolParams);
		AptnCommandBuffer copyCmd = Apparition::AllocateCommandBuffer(cmdPool);
		Apparition::BeginCommandBuffer(copyCmd);

		// Create a host-visible staging buffer that contains the raw image data
		AptnBufferCreationParams bufferParams
		{
			.size = ktxTextureSize,
			.usage = AptnBufferUsageFlags::TransferSrc,
			.supportsMappedMemory = true
		};
		AptnBuffer stagingBuffer = Apparition::CreateBuffer(device, bufferParams);

		// Copy texture data into staging buffer
		void* data = Apparition::MapBuffer(stagingBuffer);
		memcpy(data, ktxTextureData, ktxTextureSize);
		Apparition::UnmapBuffer(stagingBuffer);

		// Setup buffer copy regions for each mip level
		DynamicArray<AptnBufferToImageCopyOutline> bufferCopyRegions;
		bufferCopyRegions.Reserve(mipLevels);

		for (uint32_t i = 0; i < mipLevels; i++) {
			ktx_size_t offset;
			KTX_error_code result = ktxTexture_GetImageOffset(ktxTexture, i, 0, 0, &offset);
			Assert(result == KTX_SUCCESS);
			AptnBufferToImageCopyOutline bufferCopyRegion
			{
				.bufferOffset = offset,
				.aspect = AptnImageAspectFlags::Color,
				.mipLevel = i,
				.imgWidth = std::max(1u, ktxTexture->baseWidth >> i),
				.imgHeight = std::max(1u, ktxTexture->baseHeight >> i)
			};
			bufferCopyRegions.Add(bufferCopyRegion);
		}

		AptnImageCreationParams imageParams
		{
			.width = width,
			.height = height,
			.format = format,
			.mipLevels = mipLevels,
			.usageFlags = imageUsageFlags | AptnImageUsageFlags::TransferDst
		};
		image = Apparition::CreateImage(device, imageParams);

		// Image barrier for optimal image (target)
		// Optimal image will be used as destination for the copy
		{
			AptnImageMemoryBarrierDesc imageBarrier
			{
				.image = image,
				.access = AptnImageAccess::TransferDst,
				.aspect = AptnImageAspectFlags::Color,
				.mipLevelCount = mipLevels
			};
			Apparition::ImageMemoryBarrier(copyCmd, imageBarrier);
		}

		AptnBufferRegionsToImageCopyDesc bufferRegionCopy
		{
			.srcBuffer = stagingBuffer,
			.dstImage = image,
			.outlines = MOVE(bufferCopyRegions)
		};
		// Copy mip levels from staging buffer
		Apparition::CopyBufferRegionsToImage(copyCmd, bufferRegionCopy);

		// Change texture image layout to shader read after all mip levels have been copied
		this->imageAccess = imageAccess;
		{
			AptnImageMemoryBarrierDesc imageBarrier
			{
				.image = image,
				.access = imageAccess,
				.aspect = AptnImageAspectFlags::Color,
				.mipLevelCount = mipLevels
			};
			Apparition::ImageMemoryBarrier(copyCmd, imageBarrier);
		}

		Apparition::EndCommandBuffer(copyCmd);
		Apparition::SubmitCommandBuffer(copyQueue, copyCmd);
		Apparition::WaitForIdle(copyQueue);

		// Clean up staging resources
		Apparition::FreeCommandBuffer(copyCmd);
		Apparition::DestroyCommandPool(cmdPool);
		Apparition::DestroyBuffer(stagingBuffer);

		ktxTexture_Destroy(ktxTexture);

		// Create a default sampler
		AptnSamplerCreationParams samplerParams
		{
			.addressModeU = AptnSamplerAddressMode::Repeat,
			.addressModeV = AptnSamplerAddressMode::Repeat,
			// TODO - WE NEED TO ENABLE THIS IF IT'S ENABLED
			.maxAnisotropy = 1.f,
			.minLod = 0,
			.maxLod = (f32)mipLevels
		};
		sampler = Apparition::CreateSampler(device, samplerParams);

		// Create image view
		// Textures are not directly accessed by the shaders and
		// are abstracted by image views containing additional
		// information and sub resource ranges
		AptnImageViewCreationParams viewParams
		{
			.format = format,
			.aspect = AptnImageAspectFlags::Color,
			.mipCount = mipLevels,
			.baseMipLevel = 0
		};
		view = Apparition::CreateImageView(image, viewParams);

		// Update descriptor image info member that can be used for setting up descriptor sets
		updateDescriptor();
	}

	/**
	* Creates a 2D texture from a buffer
	*
	* @param buffer Buffer containing texture data to upload
	* @param bufferSize Size of the buffer in machine units
	* @param width Width of the texture to create
	* @param height Height of the texture to create
	* @param format Vulkan format of the image data stored in the file
	* @param device Vulkan device to create the texture on
	* @param copyQueue Queue used for the texture staging copy commands (must support transfer)
	* @param (Optional) filter Texture filtering for the sampler (defaults to VK_FILTER_LINEAR)
	* @param (Optional) imageUsageFlags Usage flags for the texture's image (defaults to VK_IMAGE_USAGE_SAMPLED_BIT)
	* @param (Optional) imageLayout Usage layout for the texture (defaults VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	*/
	void Texture2D::fromBuffer(void* buffer, VkDeviceSize bufferSize, AptnImageFormat format, uint32_t texWidth, uint32_t texHeight, AptnDevice device, AptnQueue copyQueue, AptnSamplerFilter filter, AptnImageUsageFlags imageUsageFlags, AptnImageAccess imageAccess)
	{
		Assert(buffer);

		this->device = device;
		width = texWidth;
		height = texHeight;
		mipLevels = 1;

		// Create a host-visible staging buffer that contains the raw image data
		AptnBufferCreationParams bufferParams
		{
			.size = bufferSize,
			.usage = AptnBufferUsageFlags::TransferSrc,
			.supportsMappedMemory = true
		};
		AptnBuffer stagingBuffer = Apparition::CreateBuffer(device, bufferParams);

		// Copy texture data into staging buffer
		void* data = Apparition::MapBuffer(stagingBuffer);
		memcpy(data, buffer, bufferSize);
		Apparition::UnmapBuffer(stagingBuffer);

		AptnBufferToImageCopyDesc bufferCopyRegion
		{
			.outline
			{
				.bufferOffset = 0,
				.aspect = AptnImageAspectFlags::Color,
				.mipLevel = 0,
				.imgWidth = width,
				.imgHeight = height
			}
		};

		AptnImageCreationParams imageParams
		{
			.width = width,
			.height = height,
			.format = format,
			.mipLevels = mipLevels,
			.usageFlags = imageUsageFlags | AptnImageUsageFlags::TransferDst
		};
		image = Apparition::CreateImage(device, imageParams);

		// Use a separate command buffer for texture loading
		AptnCommandPoolCreationParams poolParams
		{
			.queueIndex = Apparition::GetQueueIndex(copyQueue),
			.canResetCommandBuffers = false
		};
		AptnCommandPool cmdPool = Apparition::CreateCommandPool(device, poolParams);
		AptnCommandBuffer copyCmd = Apparition::AllocateCommandBuffer(cmdPool);
		Apparition::BeginCommandBuffer(copyCmd);

		// Image barrier for optimal image (target)
		// Optimal image will be used as destination for the copy
		{
			AptnImageMemoryBarrierDesc imageBarrier
			{
				.image = image,
				.access = AptnImageAccess::TransferDst,
				.aspect = AptnImageAspectFlags::Color,
				.mipLevelCount = mipLevels
			};
			Apparition::ImageMemoryBarrier(copyCmd, imageBarrier);
		}
		// Copy mip levels from staging buffer
		bufferCopyRegion.srcBuffer = stagingBuffer;
		bufferCopyRegion.dstImage = image;
		Apparition::CopyBufferToImage(copyCmd, bufferCopyRegion);

		// Change texture image layout to shader read after all mip levels have been copied
		this->imageAccess = imageAccess;
		{
			AptnImageMemoryBarrierDesc imageBarrier
			{
				.image = image,
				.access = imageAccess,
				.aspect = AptnImageAspectFlags::Color,
				.mipLevelCount = mipLevels
			};
			Apparition::ImageMemoryBarrier(copyCmd, imageBarrier);
		}

		Apparition::EndCommandBuffer(copyCmd);
		Apparition::SubmitCommandBuffer(copyQueue, copyCmd);
		Apparition::WaitForIdle(copyQueue);

		// Clean up staging resources
		Apparition::FreeCommandBuffer(copyCmd);
		Apparition::DestroyCommandPool(cmdPool);
		Apparition::DestroyBuffer(stagingBuffer);

		// Create sampler
		AptnSamplerCreationParams samplerParams
		{
			.filter = filter,
			.addressModeU = AptnSamplerAddressMode::Repeat,
			.addressModeV = AptnSamplerAddressMode::Repeat,
			// TODO - WE NEED TO ENABLE THIS IF IT'S ENABLED
			.maxAnisotropy = 1.f,
			.minLod = 0,
			.maxLod = 0
		};
		sampler = Apparition::CreateSampler(device, samplerParams);

		// Create image view
		AptnImageViewCreationParams viewParams
		{
			.format = format,
			.aspect = AptnImageAspectFlags::Color,
			.mipCount = 1,
			.baseMipLevel = 0
		};
		view = Apparition::CreateImageView(image, viewParams);

		// Update descriptor image info member that can be used for setting up descriptor sets
		updateDescriptor();
	}

	/**
	* Load a 2D texture array including all mip levels
	*
	* @param filename File to load (supports .ktx)
	* @param format Vulkan format of the image data stored in the file
	* @param device Vulkan device to create the texture on
	* @param copyQueue Queue used for the texture staging copy commands (must support transfer)
	* @param (Optional) imageUsageFlags Usage flags for the texture's image (defaults to VK_IMAGE_USAGE_SAMPLED_BIT)
	* @param (Optional) imageLayout Usage layout for the texture (defaults VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	*
	*/
	/*
	void Texture2DArray::loadFromFile(std::string filename, Apparition::ImageFormat::Type format, Apparition::Device device, Apparition::Queue copyQueue, Apparition::ImageUsageFlags imageUsageFlags, Apparition::ImageAccess::Type imageAccess)
	{
		ktxTexture* ktxTexture;
		ktxResult result = loadKTXFile(filename, &ktxTexture);
		assert(result == KTX_SUCCESS);

		this->device = device;
		width = ktxTexture->baseWidth;
		height = ktxTexture->baseHeight;
		layerCount = ktxTexture->numLayers;
		mipLevels = ktxTexture->numLevels;

		ktx_uint8_t *ktxTextureData = ktxTexture_GetData(ktxTexture);
		ktx_size_t ktxTextureSize = ktxTexture_GetSize(ktxTexture);

		// Create a host-visible staging buffer that contains the raw image data
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingMemory;

		VkBufferCreateInfo bufferCreateInfo = vks::initializers::bufferCreateInfo();
		bufferCreateInfo.size = ktxTextureSize;
		// This buffer is used as a transfer source for the buffer copy
		bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK_RESULT(vkCreateBuffer(device->logicalDevice, &bufferCreateInfo, nullptr, &stagingBuffer));

		// Get memory requirements for the staging buffer (alignment, memory type bits)
		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(device->logicalDevice, stagingBuffer, &memReqs);
		VkMemoryAllocateInfo memAllocInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memReqs.size,
			.memoryTypeIndex = device->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
		};
		VK_CHECK_RESULT(vkAllocateMemory(device->logicalDevice, &memAllocInfo, nullptr, &stagingMemory));
		VK_CHECK_RESULT(vkBindBufferMemory(device->logicalDevice, stagingBuffer, stagingMemory, 0));

		// Copy texture data into staging buffer
		uint8_t *data{ nullptr };
		VK_CHECK_RESULT(vkMapMemory(device->logicalDevice, stagingMemory, 0, memReqs.size, 0, (void **)&data));
		memcpy(data, ktxTextureData, ktxTextureSize);
		vkUnmapMemory(device->logicalDevice, stagingMemory);

		// Setup buffer copy regions for each layer including all of its miplevels
		std::vector<VkBufferImageCopy> bufferCopyRegions;

		for (uint32_t layer = 0; layer < layerCount; layer++) {
			for (uint32_t level = 0; level < mipLevels; level++) {
				ktx_size_t offset;
				KTX_error_code result = ktxTexture_GetImageOffset(ktxTexture, level, layer, 0, &offset);
				assert(result == KTX_SUCCESS);
				VkBufferImageCopy bufferCopyRegion{
					.bufferOffset = offset,
					.imageSubresource {
						.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
						.mipLevel = level,
						.baseArrayLayer = layer,
						.layerCount = 1,
					},
					.imageExtent {
						.width = ktxTexture->baseWidth >> level,
						.height = ktxTexture->baseHeight >> level,
						.depth = 1,
					}
				};
				bufferCopyRegions.push_back(bufferCopyRegion);
			}
		}

		// Create optimal tiled target image
		VkImageCreateInfo imageCreateInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = format,
			.extent = {.width = width, .height = height, .depth = 1 },
			.mipLevels = mipLevels,
			.arrayLayers = layerCount,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = imageUsageFlags,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		};
		// Ensure that the TRANSFER_DST bit is set for staging
		if (!(imageCreateInfo.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)) {
			imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}
		VK_CHECK_RESULT(vkCreateImage(device->logicalDevice, &imageCreateInfo, nullptr, &image));

		vkGetImageMemoryRequirements(device->logicalDevice, image, &memReqs);
		memAllocInfo.allocationSize = memReqs.size;
		memAllocInfo.memoryTypeIndex = device->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(device->logicalDevice, &memAllocInfo, nullptr, &deviceMemory));
		VK_CHECK_RESULT(vkBindImageMemory(device->logicalDevice, image, deviceMemory, 0));

		// Use a separate command buffer for texture loading
		VkCommandBuffer copyCmd = device->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
		// Image barrier for optimal image (target)
		// Set initial layout for all array layers (faces) of the optimal (target) tiled texture
		VkImageSubresourceRange subresourceRange{ .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = mipLevels, .layerCount = layerCount };
		vks::tools::setImageLayout(copyCmd, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);
		// Copy the layers and mip levels from the staging buffer to the optimal tiled image
		vkCmdCopyBufferToImage(copyCmd, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());
		// Change texture image layout to shader read after all faces have been copied
		this->imageLayout = imageLayout;
		vks::tools::setImageLayout(copyCmd, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageLayout, subresourceRange);
		device->flushCommandBuffer(copyCmd, copyQueue);

		// Create sampler
		VkSamplerCreateInfo samplerCreateInfo{
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.addressModeV = samplerCreateInfo.addressModeU,
			.addressModeW = samplerCreateInfo.addressModeU,
			.mipLodBias = 0.0f,
			.anisotropyEnable = device->enabledFeatures.samplerAnisotropy,
			.maxAnisotropy = device->enabledFeatures.samplerAnisotropy ? device->properties.limits.maxSamplerAnisotropy : 1.0f,
			.compareOp = VK_COMPARE_OP_NEVER,
			.minLod = 0.0f,
			.maxLod = (float)mipLevels,
			.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
		};
		VK_CHECK_RESULT(vkCreateSampler(device->logicalDevice, &samplerCreateInfo, nullptr, &sampler));

		// Create image view
		VkImageViewCreateInfo viewCreateInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
			.format = format,
			.subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = mipLevels, .baseArrayLayer = 0, .layerCount = layerCount },
		};
		VK_CHECK_RESULT(vkCreateImageView(device->logicalDevice, &viewCreateInfo, nullptr, &view));

		// Clean up staging resources
		ktxTexture_Destroy(ktxTexture);
		vkDestroyBuffer(device->logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(device->logicalDevice, stagingMemory, nullptr);

		// Update descriptor image info member that can be used for setting up descriptor sets
		updateDescriptor();
	}
	//*/

	/**
	* Load a cubemap texture including all mip levels from a single file
	*
	* @param filename File to load (supports .ktx)
	* @param format Vulkan format of the image data stored in the file
	* @param device Vulkan device to create the texture on
	* @param copyQueue Queue used for the texture staging copy commands (must support transfer)
	* @param (Optional) imageUsageFlags Usage flags for the texture's image (defaults to VK_IMAGE_USAGE_SAMPLED_BIT)
	* @param (Optional) imageLayout Usage layout for the texture (defaults VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	*
	*/
	/*
	void TextureCubeMap::loadFromFile(std::string filename, Apparition::ImageFormat::Type format, Apparition::Device device, Apparition::Queue copyQueue, Apparition::ImageUsageFlags imageUsageFlags, Apparition::ImageAccess::Type imageAccess)
	{
		ktxTexture* ktxTexture;
		ktxResult result = loadKTXFile(filename, &ktxTexture);
		Assert(result == KTX_SUCCESS);

		this->device = device;
		width = ktxTexture->baseWidth;
		height = ktxTexture->baseHeight;
		mipLevels = ktxTexture->numLevels;

		ktx_uint8_t *ktxTextureData = ktxTexture_GetData(ktxTexture);
		ktx_size_t ktxTextureSize = ktxTexture_GetSize(ktxTexture);

		// Create a host-visible staging buffer that contains the raw image data
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingMemory;

		VkBufferCreateInfo bufferCreateInfo{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = ktxTextureSize,
			.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE
		};
		VK_CHECK_RESULT(vkCreateBuffer(device->logicalDevice, &bufferCreateInfo, nullptr, &stagingBuffer));

		// Get memory requirements for the staging buffer (alignment, memory type bits)
		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(device->logicalDevice, stagingBuffer, &memReqs);
		VkMemoryAllocateInfo memAllocInfo{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memReqs.size,
			.memoryTypeIndex = device->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
		};
		VK_CHECK_RESULT(vkAllocateMemory(device->logicalDevice, &memAllocInfo, nullptr, &stagingMemory));
		VK_CHECK_RESULT(vkBindBufferMemory(device->logicalDevice, stagingBuffer, stagingMemory, 0));

		// Copy texture data into staging buffer
		uint8_t *data{ nullptr };
		VK_CHECK_RESULT(vkMapMemory(device->logicalDevice, stagingMemory, 0, memReqs.size, 0, (void **)&data));
		memcpy(data, ktxTextureData, ktxTextureSize);
		vkUnmapMemory(device->logicalDevice, stagingMemory);

		// Setup buffer copy regions for each face including all of its mip levels
		std::vector<VkBufferImageCopy> bufferCopyRegions;
		for (uint32_t face = 0; face < 6; face++) {
			for (uint32_t level = 0; level < mipLevels; level++) {
				ktx_size_t offset;
				KTX_error_code result = ktxTexture_GetImageOffset(ktxTexture, level, 0, face, &offset);
				assert(result == KTX_SUCCESS);
				VkBufferImageCopy bufferCopyRegion{
					.bufferOffset = offset,
					.imageSubresource = {
						.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
						.mipLevel = level,
						.baseArrayLayer = face,
						.layerCount = 1
					},
					.imageExtent = {
						.width = ktxTexture->baseWidth >> level,
						.height = ktxTexture->baseHeight >> level,
						.depth = 1
					},
				};
				bufferCopyRegions.push_back(bufferCopyRegion);
			}
		}

		// Create optimal tiled target image
		VkImageCreateInfo imageCreateInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			// This flag is required for cube map images
			.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = format,
			.extent = { .width = width, .height = height, .depth = 1 },
			.mipLevels = mipLevels,
			// Cube faces count as array layers in Vulkan
			.arrayLayers = 6,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = imageUsageFlags,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		};
		// Ensure that the TRANSFER_DST bit is set for staging
		if (!(imageCreateInfo.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)){
			imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}
		VK_CHECK_RESULT(vkCreateImage(device->logicalDevice, &imageCreateInfo, nullptr, &image));

		vkGetImageMemoryRequirements(device->logicalDevice, image, &memReqs);
		memAllocInfo.allocationSize = memReqs.size;
		memAllocInfo.memoryTypeIndex = device->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(device->logicalDevice, &memAllocInfo, nullptr, &deviceMemory));
		VK_CHECK_RESULT(vkBindImageMemory(device->logicalDevice, image, deviceMemory, 0));

		// Use a separate command buffer for texture loading
		VkCommandBuffer copyCmd = device->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
		// Image barrier for optimal image (target)
		// Set initial layout for all array layers (faces) of the optimal (target) tiled texture
		VkImageSubresourceRange subresourceRange{ .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = mipLevels, .layerCount = 6 };
		vks::tools::setImageLayout(copyCmd, image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);
		// Copy the cube map faces from the staging buffer to the optimal tiled image
		vkCmdCopyBufferToImage(copyCmd, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());
		// Change texture image layout to shader read after all faces have been copied
		this->imageLayout = imageLayout;
		vks::tools::setImageLayout(copyCmd, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageLayout, subresourceRange);
		device->flushCommandBuffer(copyCmd, copyQueue);

		// Create sampler
		VkSamplerCreateInfo samplerCreateInfo{
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.addressModeV = samplerCreateInfo.addressModeU,
			.addressModeW = samplerCreateInfo.addressModeU,
			.mipLodBias = 0.0f,
			.anisotropyEnable = device->enabledFeatures.samplerAnisotropy,
			.maxAnisotropy = device->enabledFeatures.samplerAnisotropy ? device->properties.limits.maxSamplerAnisotropy : 1.0f,
			.compareOp = VK_COMPARE_OP_NEVER,
			.minLod = 0.0f,
			.maxLod = (float)mipLevels,
			.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
		};
		VK_CHECK_RESULT(vkCreateSampler(device->logicalDevice, &samplerCreateInfo, nullptr, &sampler));

		// Create image view
		VkImageViewCreateInfo viewCreateInfo{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = image,
			.viewType = VK_IMAGE_VIEW_TYPE_CUBE,
			.format = format,
			.subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = mipLevels, .baseArrayLayer = 0, .layerCount = 6 },
		};
		VK_CHECK_RESULT(vkCreateImageView(device->logicalDevice, &viewCreateInfo, nullptr, &view));

		// Clean up staging resources
		ktxTexture_Destroy(ktxTexture);
		vkDestroyBuffer(device->logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(device->logicalDevice, stagingMemory, nullptr);

		// Update descriptor image info member that can be used for setting up descriptor sets
		updateDescriptor();
	}
	//*/
}
