/*
* Vulkan glTF model and texture loading class based on tinyglTF (https://github.com/syoyo/tinygltf)
*
* Copyright (C) 2018-2026 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

/*
 * Note that this isn't a complete glTF loader and not all features of the glTF 2.0 spec are supported
 * For details on how glTF 2.0 works, see the official spec at https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
 *
 * If you are looking for a complete glTF implementation, check out https://github.com/SaschaWillems/Vulkan-glTF-PBR/
 */

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include <iostream>

#include "VulkanglTFModel.h"

#include "File/FileSystem.hpp"

#include "Apparition/CommandBufferCommands.h"

AptnDescriptorSetLayout vkglTF::descriptorSetLayoutImage{ AptnInvalidHandle };
AptnDescriptorSetLayout vkglTF::descriptorSetLayoutUbo{ AptnInvalidHandle };
VkMemoryPropertyFlags vkglTF::memoryPropertyFlags = 0;
uint32_t vkglTF::descriptorBindingFlags = vkglTF::DescriptorBindingFlags::ImageBaseColor;

/*
	We use a custom image loading function with tinyglTF, so we can do custom stuff loading ktx textures
*/
bool loadImageDataFunc(tinygltf::Image* image, const int imageIndex, std::string* error, std::string* warning, int req_width, int req_height, const unsigned char* bytes, int size, void* userData)
{
	// KTX files will be handled by our own code
	if (image->uri.find_last_of(".") != std::string::npos) {
		if (image->uri.substr(image->uri.find_last_of(".") + 1) == "ktx") {
			return true;
		}
	}

	return tinygltf::LoadImageData(image, imageIndex, error, warning, req_width, req_height, bytes, size, userData);
}

bool loadImageDataFuncEmpty(tinygltf::Image* /*image*/, const int /*imageIndex*/, std::string* /*error*/, std::string* /*warning*/, int /*req_width*/, int /*req_height*/, const unsigned char* /*bytes*/, int /*size*/, void* /*userData*/) 
{
	// This function will be used for samples that don't require images to be loaded
	return true;
}

AptnImageFormat VkFormatToApparitionFormat(VkFormat format)
{
	switch (format)
	{
	case VK_FORMAT_R8G8B8_UNORM:
		return AptnImageFormat::RGB_8norm;
	case VK_FORMAT_R8G8B8_UINT:
		return AptnImageFormat::RGB_8u;
	case VK_FORMAT_R16G16B16_SFLOAT:
		return AptnImageFormat::RGB_16f;
	case VK_FORMAT_B8G8R8_UNORM:
		return AptnImageFormat::BGR_8norm;
	case VK_FORMAT_R8G8B8A8_UNORM:
		return AptnImageFormat::RGBA_8norm;
	case VK_FORMAT_R8G8B8A8_UINT:
		return AptnImageFormat::RGBA_8u;
	case VK_FORMAT_R16G16B16A16_SFLOAT:
		return AptnImageFormat::RGBA_16f;
	case VK_FORMAT_B8G8R8A8_UNORM:
		return AptnImageFormat::BGRA_8norm;
	case VK_FORMAT_R8_UNORM:
		return AptnImageFormat::Gray_8norm;
	case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
		return AptnImageFormat::BC1;
	case VK_FORMAT_BC3_UNORM_BLOCK:
		return AptnImageFormat::BC3;
	case VK_FORMAT_BC7_UNORM_BLOCK:
		return AptnImageFormat::BC7;
	case VK_FORMAT_D32_SFLOAT:
		return AptnImageFormat::D_32f;
	case VK_FORMAT_D32_SFLOAT_S8_UINT:
		return AptnImageFormat::DS_32f_8u;
	case VK_FORMAT_D24_UNORM_S8_UINT:
		return AptnImageFormat::DS_24f_8u;
	case VK_FORMAT_UNDEFINED:
	default:
		Assert(false);
	}

	return AptnImageFormat::Invalid;
}

/*
	glTF texture loading class
*/

void vkglTF::Texture::updateDescriptor()
{
	descriptor.sampler = sampler;
	descriptor.imageView = view;
	descriptor.access = access;
}

void vkglTF::Texture::destroy()
{
	if (IsValid(device))
	{
		Apparition::DestroyCommandPool(commandPool);
		Apparition::DestroyImageView(view);
		Apparition::DestroyImage(image);
		Apparition::DestroySampler(sampler);
	}
}

void vkglTF::Texture::fromglTfImage(tinygltf::Image &gltfimage, std::string path, AptnDevice device, AptnQueue copyQueue)
{
	this->device = device;
	AptnCommandPoolCreationParams params
	{
		.queueIndex = Apparition::GetQueueIndex(copyQueue)
	};
	this->commandPool = Apparition::CreateCommandPool(device, params);

	bool isKtx = false;
	// Image points to an external ktx file
	if (gltfimage.uri.find_last_of(".") != std::string::npos) {
		if (gltfimage.uri.substr(gltfimage.uri.find_last_of(".") + 1) == "ktx") {
			isKtx = true;
		}
	}

	AptnImageFormat format;

	if (!isKtx) {
		// Texture was loaded using STB_Image

		unsigned char* buffer = nullptr;
		u64 bufferSize = 0;
		bool deleteBuffer = false;
		if (gltfimage.component == 3) {
			// Most devices don't support RGB only on Vulkan so convert if necessary
			// TODO: Check actual format support and transform only if required
			bufferSize = gltfimage.width * gltfimage.height * 4;
			buffer = new unsigned char[bufferSize];
			unsigned char* rgba = buffer;
			unsigned char* rgb = &gltfimage.image[0];
			for (int32_t i = 0; i < gltfimage.width * gltfimage.height; ++i) {
				for (int32_t j = 0; j < 3; ++j) {
					rgba[j] = rgb[j];
				}
				rgba += 4;
				rgb += 3;
			}
			deleteBuffer = true;
		}
		else {
			buffer = &gltfimage.image[0];
			bufferSize = gltfimage.image.size();
		}
		assert(buffer);

		format = AptnImageFormat::RGBA_8norm;

		width = gltfimage.width;
		height = gltfimage.height;
		mipLevels = static_cast<uint32_t>(floor(log2(std::max(width, height))) + 1.0);

		/*VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(device->physicalDevice, format, &formatProperties);
		assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT);
		assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT);*/

		AptnBufferCreationParams buffParams
		{
			.usage = AptnBufferUsageFlags::TransferSrc,
			.size = bufferSize,
			.supportsMappedMemory = true
		};
		AptnBuffer stagingBuffer = Apparition::CreateBuffer(device, buffParams);

		void* data = Apparition::MapBuffer(stagingBuffer);
		memcpy(data, buffer, bufferSize);
		Apparition::UnmapBuffer(stagingBuffer);

		AptnImageCreationParams imgParams
		{
			.width = width,
			.height = height,
			.format = format,
			.mipLevels = mipLevels,
			.usageFlags = AptnImageUsageFlags::TransferSrc |
						AptnImageUsageFlags::TransferDst |
						AptnImageUsageFlags::Sampled
		};
		image = Apparition::CreateImage(device, imgParams);

		// TODO - Could make this a function call instead?
		Assert(IsValid(commandPool));
		AptnCommandBufferAllocParams cmdBuffParams;
		AptnCommandBuffer copyCmd = Apparition::AllocateCommandBuffer(commandPool, cmdBuffParams);
		Apparition::BeginCommandBuffer(copyCmd);

		{
			AptnImageMemoryBarrierDesc barrierDesc
			{
				.image = image,
				.access = AptnImageAccess::TransferDst,
				.aspect = AptnImageAspectFlags::Color
			};
			Apparition::ImageMemoryBarrier(copyCmd, barrierDesc);
		}

		AptnBufferToImageCopyDesc copyDesc
		{
			.srcBuffer = stagingBuffer,
			.dstImage = image,
			.outline = 
			{
				.aspect = AptnImageAspectFlags::Color,
				.mipLevel = 0,
				.imgWidth = width,
				.imgHeight = height
			}
		};
		Apparition::CopyBufferToImage(copyCmd, copyDesc);

		{
			AptnImageMemoryBarrierDesc barrierDesc
			{
				.image = image,
				.access = AptnImageAccess::TransferSrc,
				.aspect = AptnImageAspectFlags::Color
			};
			Apparition::ImageMemoryBarrier(copyCmd, barrierDesc);
		}
		Apparition::EndCommandBuffer(copyCmd);
		Apparition::SubmitCommandBuffer(copyQueue, copyCmd);
		Apparition::WaitForIdle(copyQueue);
		Apparition::FreeCommandBuffer(copyCmd);

		Apparition::DestroyBuffer(stagingBuffer);

		// Generate the mip chain (glTF uses jpg and png, so we need to create this manually)
		AptnCommandBuffer blitCmd = Apparition::AllocateCommandBuffer(commandPool, cmdBuffParams);
		Apparition::BeginCommandBuffer(blitCmd);
		for (uint32_t i = 1; i < mipLevels; i++) {
			AptnBlitImageDesc blitDesc{};
			// Src blit desc
			blitDesc.srcImage = image;
			blitDesc.srcAspect = AptnImageAspectFlags::Color;
			blitDesc.srcMipLevel = i - 1;
			blitDesc.srcOffsetX[1] = (i32)(width >> (i - 1));
			blitDesc.srcOffsetY[1] = (i32)(height >> (i - 1));
			// Dst blit desc
			blitDesc.dstImage = image;
			blitDesc.dstAspect = AptnImageAspectFlags::Color;
			blitDesc.dstMipLevel = i;
			blitDesc.dstOffsetX[1] = (i32)(width >> i);
			blitDesc.dstOffsetY[1] = (i32)(height >> i);

			{
				AptnImageMemoryBarrierDesc barrierDesc
				{
					.image = image,
					.access = AptnImageAccess::TransferDst,
					.aspect = AptnImageAspectFlags::Color,
					.baseMipLevel = i,
					.mipLevelCount = 1
				};
				Apparition::ImageMemoryBarrier(copyCmd, barrierDesc);
			}

			Apparition::BlitImage(blitCmd, blitDesc);
			{
				AptnImageMemoryBarrierDesc barrierDesc
				{
					.image = image,
					.access = AptnImageAccess::TransferSrc,
					.aspect = AptnImageAspectFlags::Color,
					.baseMipLevel = i,
					.mipLevelCount = 1
				};
				Apparition::ImageMemoryBarrier(copyCmd, barrierDesc);
			}
		}

		AptnImageMemoryBarrierDesc barrierDesc
		{
			.image = image,
			.access = AptnImageAccess::ColorRead,
			.aspect = AptnImageAspectFlags::Color,
			.baseMipLevel = 0,
			.mipLevelCount = mipLevels
		};
		Apparition::ImageMemoryBarrier(copyCmd, barrierDesc);

        if (deleteBuffer) {
            delete[] buffer;
        }

		Apparition::EndCommandBuffer(blitCmd);
		Apparition::SubmitCommandBuffer(copyQueue, blitCmd);
		Apparition::WaitForIdle(copyQueue);
		Apparition::FreeCommandBuffer(blitCmd);
	}
	else {
		// Texture is stored in an external ktx file
		std::string filename = path + "/" + gltfimage.uri;

		ktxTexture* ktxTexture;

		ktxResult result = KTX_SUCCESS;
#if defined(__ANDROID__)
		AAsset* asset = AAssetManager_open(androidApp->activity->assetManager, filename.c_str(), AASSET_MODE_STREAMING);
		if (!asset) {
			vks::tools::exitFatal("Could not load texture from " + filename + "\n\nMake sure the assets submodule has been checked out and is up-to-date.", -1);
		}
		size_t size = AAsset_getLength(asset);
		assert(size > 0);
		ktx_uint8_t* textureData = new ktx_uint8_t[size];
		AAsset_read(asset, textureData, size);
		AAsset_close(asset);
		result = ktxTexture_CreateFromMemory(textureData, size, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
		delete[] textureData;
#else
		if (!FileSystem::DoesFileExist(filename.c_str())) {
			//vks::tools::exitFatal("Could not load texture from " + filename + "\n\nMake sure the assets submodule has been checked out and is up-to-date.", -1);
			Assert(false);
			return;
		}
		result = ktxTexture_CreateFromNamedFile(filename.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);
#endif		
		assert(result == KTX_SUCCESS);

		this->device = device;
		width = ktxTexture->baseWidth;
		height = ktxTexture->baseHeight;
		mipLevels = ktxTexture->numLevels;

		ktx_uint8_t* ktxTextureData = ktxTexture_GetData(ktxTexture);
		ktx_size_t ktxTextureSize = ktxTexture_GetSize(ktxTexture);
		VkFormat vkFormat = ktxTexture_GetVkFormat(ktxTexture);
		format = VkFormatToApparitionFormat(vkFormat);

		// Get device properties for the requested texture format
		//VkFormatProperties formatProperties;
		//vkGetPhysicalDeviceFormatProperties(device->physicalDevice, format, &formatProperties);

		Assert(IsValid(commandPool));
		AptnCommandBufferAllocParams cmdBuffParams;
		AptnCommandBuffer copyCmd = Apparition::AllocateCommandBuffer(commandPool, cmdBuffParams);
		Apparition::BeginCommandBuffer(copyCmd);

		AptnBufferCreationParams bufferParams
		{
			.usage = AptnBufferUsageFlags::TransferSrc,
			.size = ktxTextureSize,
			.supportsMappedMemory = true
		};
		AptnBuffer stagingBuffer = Apparition::CreateBuffer(device, bufferParams);

		void* data = Apparition::MapBuffer(stagingBuffer);
		memcpy(data, ktxTextureData, ktxTextureSize);
		Apparition::UnmapBuffer(stagingBuffer);


		DynamicArray<AptnBufferToImageCopyOutline> bufferCopyOutlines;
		for (uint32_t i = 0; i < mipLevels; i++)
		{
			ktx_size_t offset;
			KTX_error_code result = ktxTexture_GetImageOffset(ktxTexture, i, 0, 0, &offset);
			assert(result == KTX_SUCCESS);
			AptnBufferToImageCopyOutline outline
			{
				.aspect = AptnImageAspectFlags::Color,
				.mipLevel = i,
				.imgWidth = std::max(1u, ktxTexture->baseWidth >> i),
				.imgHeight = std::max(1u, ktxTexture->baseHeight >> i)
			};
			bufferCopyOutlines.Add(outline);
		}

		AptnImageCreationParams imgParams
		{
			.width = width,
			.height = height,
			.format = format,
			.mipLevels = mipLevels,
			.usageFlags = AptnImageUsageFlags::TransferDst | AptnImageUsageFlags::Sampled
		};
		AptnImage image = Apparition::CreateImage(device, imgParams);

		{
			AptnImageMemoryBarrierDesc barrierDesc
			{
				.image = image,
				.access = AptnImageAccess::TransferDst,
				.aspect = AptnImageAspectFlags::Color,
				.baseMipLevel = 0,
				.mipLevelCount = mipLevels
			};
			Apparition::ImageMemoryBarrier(copyCmd, barrierDesc);
		}

		AptnBufferRegionsToImageCopyDesc copyDesc
		{
			.srcBuffer = stagingBuffer,
			.dstImage = image,
			.outlines = bufferCopyOutlines
		};
		Apparition::CopyBufferRegionsToImage(copyCmd, copyDesc);

		{
			AptnImageMemoryBarrierDesc barrierDesc
			{
				.image = image,
				.access = AptnImageAccess::ColorRead,
				.aspect = AptnImageAspectFlags::Color,
				.baseMipLevel = 0,
				.mipLevelCount = mipLevels
			};
			Apparition::ImageMemoryBarrier(copyCmd, barrierDesc);
		}

		Apparition::EndCommandBuffer(copyCmd);
		Apparition::SubmitCommandBuffer(copyQueue, copyCmd);
		Apparition::WaitForIdle(copyQueue);
		Apparition::FreeCommandBuffer(copyCmd);

		access = AptnImageAccess::ColorRead;

		Apparition::DestroyBuffer(stagingBuffer);

		ktxTexture_Destroy(ktxTexture);
	}

	AptnSamplerCreationParams samplerParams
	{
		.filter = AptnSamplerFilter::Linear,
		.addressModeU = AptnSamplerAddressMode::Mirror,
		.addressModeV = AptnSamplerAddressMode::Mirror,
		.mipMode = AptnSamplerMipmapMode::Linear,
		.maxAnisotropy = 8.f,
		.maxLod = static_cast<f32>(mipLevels),
	};
	sampler = Apparition::CreateSampler(device, samplerParams);

	AptnImageViewCreationParams viewParams
	{
		.format = format,
		.aspect = AptnImageAspectFlags::Color,
		.mipCount = mipLevels
	};
	view = Apparition::CreateImageView(image, viewParams);

	descriptor.sampler = sampler;
	descriptor.imageView = view;
	descriptor.access = access;
}

/*
	glTF material
*/
void vkglTF::Material::createDescriptorSet(AptnDescriptorPool descriptorPool, AptnDescriptorSetLayout descriptorSetLayout, uint32_t descriptorBindingFlags)
{
	AptnDescriptorSetAllocParams dsParams
	{
		.layout = descriptorSetLayout
	};
	descriptorSet = Apparition::AllocateDescriptorSet(descriptorPool, dsParams);

	DynamicArray<AptnImageDescriptorInfo> imageDescriptors;
	DynamicArray<AptnUpdateDescriptorSetDesc> updateDescriptors;
	if (descriptorBindingFlags & DescriptorBindingFlags::ImageBaseColor) {
		imageDescriptors.Add(baseColorTexture->descriptor);
		AptnUpdateDescriptorSetDesc updateDesc
		{
			.descriptorSet = descriptorSet,
			.setBinding = updateDescriptors.Size(),
			.imageDescriptor = &baseColorTexture->descriptor
		};
		updateDescriptors.Add(updateDesc);
	}
	if (normalTexture && descriptorBindingFlags & DescriptorBindingFlags::ImageNormalMap) {
		imageDescriptors.Add(baseColorTexture->descriptor);
		AptnUpdateDescriptorSetDesc updateDesc
		{
			.descriptorSet = descriptorSet,
			.setBinding = updateDescriptors.Size(),
			.descriptorType = AptnDescriptor::CombinedImageSampler,
			.imageDescriptor = &baseColorTexture->descriptor
		};
		updateDescriptors.Add(updateDesc);
	}
	Apparition::UpdateDescriptorSets(updateDescriptors);
}


/*
	glTF primitive
*/
void vkglTF::Primitive::setDimensions(glm::vec3 min, glm::vec3 max) {
	dimensions.min = min;
	dimensions.max = max;
	dimensions.size = max - min;
	dimensions.center = (min + max) / 2.0f;
	dimensions.radius = glm::distance(min, max) / 2.0f;
}

/*
	glTF mesh
*/
vkglTF::Mesh::Mesh(AptnDevice device, glm::mat4 matrix) {
	this->device = device;
	this->uniformBlock.matrix = matrix;

	u64 bufferSize = sizeof(uniformBlock);
	AptnBufferCreationParams bufferParams
	{
		.usage = AptnBufferUsageFlags::UniformBuffer,
		.size = bufferSize,
		.supportsMappedMemory = true
	};
	uniformBuffer.buffer = Apparition::CreateBuffer(device, bufferParams);

	void* data = Apparition::MapBuffer(uniformBuffer.buffer);
	memcpy(data, &uniformBlock, bufferSize);
	Apparition::UnmapBuffer(uniformBuffer.buffer);

	uniformBuffer.mapped = Apparition::MapBuffer(uniformBuffer.buffer);
	uniformBuffer.descriptor = AptnBufferDescriptorInfo{ uniformBuffer.buffer, 0, sizeof(uniformBlock) };
};

vkglTF::Mesh::~Mesh() {
	Apparition::UnmapBuffer(uniformBuffer.buffer);
	Apparition::DestroyBuffer(uniformBuffer.buffer);
    for(auto primitive : primitives)
    {
        delete primitive;
    }
}

/*
	glTF node
*/
glm::mat4 vkglTF::Node::localMatrix() {
	return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * matrix;
}

glm::mat4 vkglTF::Node::getMatrix() {
	glm::mat4 m = localMatrix();
	vkglTF::Node *p = parent;
	while (p) {
		m = p->localMatrix() * m;
		p = p->parent;
	}
	return m;
}

void vkglTF::Node::update() {
	if (mesh) {
		glm::mat4 m = getMatrix();
		if (skin) {
			mesh->uniformBlock.matrix = m;
			// Update join matrices
			glm::mat4 inverseTransform = glm::inverse(m);
			for (size_t i = 0; i < skin->joints.size(); i++) {
				vkglTF::Node *jointNode = skin->joints[i];
				glm::mat4 jointMat = jointNode->getMatrix() * skin->inverseBindMatrices[i];
				jointMat = inverseTransform * jointMat;
				mesh->uniformBlock.jointMatrix[i] = jointMat;
			}
			mesh->uniformBlock.jointcount = (float)skin->joints.size();
			memcpy(mesh->uniformBuffer.mapped, &mesh->uniformBlock, sizeof(mesh->uniformBlock));
		} else {
			memcpy(mesh->uniformBuffer.mapped, &m, sizeof(glm::mat4));
		}
	}

	for (auto& child : children) {
		child->update();
	}
}

vkglTF::Node::~Node() {
	if (mesh) {
		delete mesh;
	}
	for (auto& child : children) {
		delete child;
	}
}

/*
	glTF default vertex layout with easy Vulkan mapping functions
*/

AptnVertexBindingDescription vkglTF::Vertex::vertexInputBindingDescription;
DynamicArray<AptnVertexAttributeDescription> vkglTF::Vertex::vertexInputAttributeDescriptions;

AptnVertexBindingDescription vkglTF::Vertex::inputBindingDescription(uint32_t binding) {
	return AptnVertexBindingDescription({ binding, sizeof(Vertex), AptnVertexInputRate::Vertex });
}

AptnVertexAttributeDescription vkglTF::Vertex::inputAttributeDescription(uint32_t binding, uint32_t location, VertexComponent component) {
	switch (component) {
		case VertexComponent::Position: 
			return AptnVertexAttributeDescription({ location, binding, AptnVertexInputFormat::F32_3, offsetof(Vertex, pos) });
		case VertexComponent::Normal:
			return AptnVertexAttributeDescription({ location, binding, AptnVertexInputFormat::F32_3, offsetof(Vertex, normal) });
		case VertexComponent::UV:
			return AptnVertexAttributeDescription({ location, binding, AptnVertexInputFormat::F32_2, offsetof(Vertex, uv) });
		case VertexComponent::Color:
			return AptnVertexAttributeDescription({ location, binding, AptnVertexInputFormat::F32_4, offsetof(Vertex, color) });
		case VertexComponent::Tangent:
			return AptnVertexAttributeDescription({ location, binding, AptnVertexInputFormat::F32_4, offsetof(Vertex, tangent) });
		case VertexComponent::Joint0:
			return AptnVertexAttributeDescription({ location, binding, AptnVertexInputFormat::F32_4, offsetof(Vertex, joint0) });
		case VertexComponent::Weight0:
			return AptnVertexAttributeDescription({ location, binding, AptnVertexInputFormat::F32_4, offsetof(Vertex, weight0) });
		default:
			return AptnVertexAttributeDescription({});
	}
}

DynamicArray<AptnVertexAttributeDescription> vkglTF::Vertex::inputAttributeDescriptions(uint32_t binding, const std::vector<VertexComponent> components) {
	DynamicArray<AptnVertexAttributeDescription> result;
	uint32_t location = 0;
	for (VertexComponent component : components) {
		result.Add(Vertex::inputAttributeDescription(binding, location, component));
		location++;
	}
	return result;
}

vkglTF::Texture* vkglTF::Model::getTexture(uint32_t index)
{

	if (index < textures.size()) {
		return &textures[index];
	}
	return nullptr;
}

void vkglTF::Model::createEmptyTexture(AptnQueue transferQueue)
{
	emptyTexture.device = device;
	emptyTexture.width = 1;
	emptyTexture.height = 1;
	emptyTexture.layerCount = 1;
	emptyTexture.mipLevels = 1;

	size_t bufferSize = emptyTexture.width * emptyTexture.height * 4;
	unsigned char* buffer = new unsigned char[bufferSize];
	memset(buffer, 0, bufferSize);

	AptnBufferCreationParams bufferParams
	{
		.usage = AptnBufferUsageFlags::TransferSrc,
		.size = bufferSize,
		.supportsMappedMemory = true
	};
	AptnBuffer stagingBuffer = Apparition::CreateBuffer(device, bufferParams);

	// Copy texture data into staging buffer
	void* data = Apparition::MapBuffer(stagingBuffer);
	memcpy(data, buffer, bufferSize);
	Apparition::UnmapBuffer(stagingBuffer);

	// Create optimal tiled target image
	AptnImageCreationParams imageParams
	{
		.width = emptyTexture.width,
		.height = emptyTexture.height,
		.format = AptnImageFormat::RGBA_8norm,
		.mipLevels = 1,
		.usageFlags = AptnImageUsageFlags::Sampled | AptnImageUsageFlags::TransferDst
	};
	emptyTexture.image = Apparition::CreateImage(device, imageParams);

	AptnCommandPoolCreationParams params
	{
		.queueIndex = Apparition::GetQueueIndex(transferQueue)
	};
	emptyTexture.commandPool = Apparition::CreateCommandPool(device, params);
	AptnCommandPool commandPool = emptyTexture.commandPool;
	AptnCommandBufferAllocParams cmdBuffParams;
	AptnCommandBuffer copyCmd = Apparition::AllocateCommandBuffer(commandPool, cmdBuffParams);
	Apparition::BeginCommandBuffer(copyCmd);

	{
		AptnImageMemoryBarrierDesc barrierDesc
		{
			.image = emptyTexture.image,
			.access = AptnImageAccess::TransferDst,
			.aspect = AptnImageAspectFlags::Color,
			.baseMipLevel = 0,
			.mipLevelCount = 1
		};
		Apparition::ImageMemoryBarrier(copyCmd, barrierDesc);
	}

	AptnBufferToImageCopyDesc copyDesc
	{
		.srcBuffer = stagingBuffer,
		.dstImage = emptyTexture.image,
		.outline =
		{
			.aspect = AptnImageAspectFlags::Color,
			.mipLevel = 0,
			.imgWidth = emptyTexture.width,
			.imgHeight = emptyTexture.height
		}
	};
	Apparition::CopyBufferToImage(copyCmd, copyDesc);
	{
		AptnImageMemoryBarrierDesc barrierDesc
		{
			.image = emptyTexture.image,
			.access = AptnImageAccess::ColorRead,
			.aspect = AptnImageAspectFlags::Color,
			.baseMipLevel = 0,
			.mipLevelCount = 1
		};
		Apparition::ImageMemoryBarrier(copyCmd, barrierDesc);
	}

	Apparition::EndCommandBuffer(copyCmd);
	Apparition::SubmitCommandBuffer(transferQueue, copyCmd);
	Apparition::WaitForIdle(transferQueue);
	Apparition::FreeCommandBuffer(copyCmd);
	emptyTexture.access = AptnImageAccess::ColorRead;

	// Clean up staging resources
	Apparition::DestroyBuffer(stagingBuffer);

	AptnSamplerCreationParams samplerParams
	{
		.filter = AptnSamplerFilter::Linear,
		.addressModeU = AptnSamplerAddressMode::Repeat,
		.addressModeV = AptnSamplerAddressMode::Repeat,
		.mipMode = AptnSamplerMipmapMode::Linear,
		.maxAnisotropy = 1
	};
	emptyTexture.sampler = Apparition::CreateSampler(device, samplerParams);

	AptnImageViewCreationParams viewParams
	{
		.format = AptnImageFormat::RGBA_8norm
	};
	emptyTexture.view = Apparition::CreateImageView(emptyTexture.image, viewParams);

	emptyTexture.access = AptnImageAccess::ColorRead;
	emptyTexture.descriptor.imageView = emptyTexture.view;
	emptyTexture.descriptor.sampler = emptyTexture.sampler;

	delete[] buffer;
}

/*
	glTF model loading and rendering class
*/
vkglTF::Model::~Model()
{
	if (!resourcesReleased)
	{
		releaseResources();
	}
}

void vkglTF::Model::releaseResources()
{
	if (IsValid(vertices.buffer))
	{
		Apparition::DestroyBuffer(vertices.buffer);
		Apparition::DestroyBuffer(indices.buffer);

		for (auto& texture : textures) {
			texture.destroy();
		}
		for (auto& node : nodes) {
			delete node;
		}
		for (auto& skin : skins) {
			delete skin;
		}
		if (IsValid(descriptorSetLayoutUbo)) {
			Apparition::DestroyDescriptorSetLayout(descriptorSetLayoutUbo);
		}
		if (IsValid(descriptorSetLayoutImage)) {
			Apparition::DestroyDescriptorSetLayout(descriptorSetLayoutImage);
		}
		Apparition::DestroyDescriptorPool(descriptorPool);
		emptyTexture.destroy();

		resourcesReleased = true;
	}
}

void vkglTF::Model::loadNode(vkglTF::Node *parent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer, float globalscale)
{
	vkglTF::Node *newNode = new Node{};
	newNode->index = nodeIndex;
	newNode->parent = parent;
	newNode->name = node.name;
	newNode->skinIndex = node.skin;
	newNode->matrix = glm::mat4(1.0f);

	// Generate local node matrix
	glm::vec3 translation = glm::vec3(0.0f);
	if (node.translation.size() == 3) {
		translation = glm::make_vec3(node.translation.data());
		newNode->translation = translation;
	}
	//glm::mat4 rotation = glm::mat4(1.0f);
	if (node.rotation.size() == 4) {
		glm::quat q = glm::make_quat(node.rotation.data());
		newNode->rotation = glm::mat4(q);
	}
	glm::vec3 scale = glm::vec3(1.0f);
	if (node.scale.size() == 3) {
		scale = glm::make_vec3(node.scale.data());
		newNode->scale = scale;
	}
	if (node.matrix.size() == 16) {
		newNode->matrix = glm::make_mat4x4(node.matrix.data());
		if (globalscale != 1.0f) {
			//newNode->matrix = glm::scale(newNode->matrix, glm::vec3(globalscale));
		}
	};

	// Node with children
	if (node.children.size() > 0) {
		for (size_t i = 0; i < node.children.size(); i++) {
			loadNode(newNode, model.nodes[node.children[i]], node.children[i], model, indexBuffer, vertexBuffer, globalscale);
		}
	}

	// Node contains mesh data
	if (node.mesh > -1) {
		const tinygltf::Mesh mesh = model.meshes[node.mesh];
		Mesh *newMesh = new Mesh(device, newNode->matrix);
		newMesh->name = mesh.name;
		for (size_t j = 0; j < mesh.primitives.size(); j++) {
			const tinygltf::Primitive &primitive = mesh.primitives[j];
			if (primitive.indices < 0) {
				continue;
			}
			uint32_t indexStart = static_cast<uint32_t>(indexBuffer.size());
			uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.size());
			uint32_t indexCount = 0;
			uint32_t vertexCount = 0;
			glm::vec3 posMin{};
			glm::vec3 posMax{};
			bool hasSkin = false;
			// Vertices
			{
				const float *bufferPos = nullptr;
				const float *bufferNormals = nullptr;
				const float *bufferTexCoords = nullptr;
				const float* bufferColors = nullptr;
				const float *bufferTangents = nullptr;
				uint32_t numColorComponents;
				const uint16_t *bufferJoints = nullptr;
				const float *bufferWeights = nullptr;

				// Position attribute is required
				assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

				const tinygltf::Accessor &posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView &posView = model.bufferViews[posAccessor.bufferView];
				bufferPos = reinterpret_cast<const float *>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
				posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
				posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);

				if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
					const tinygltf::Accessor &normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView &normView = model.bufferViews[normAccessor.bufferView];
					bufferNormals = reinterpret_cast<const float *>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
				}

				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
					const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
					bufferTexCoords = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
				}

				if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
				{
					const tinygltf::Accessor& colorAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
					const tinygltf::BufferView& colorView = model.bufferViews[colorAccessor.bufferView];
					// Color buffer are either of type vec3 or vec4
					numColorComponents = colorAccessor.type == TINYGLTF_PARAMETER_TYPE_FLOAT_VEC3 ? 3 : 4;
					bufferColors = reinterpret_cast<const float*>(&(model.buffers[colorView.buffer].data[colorAccessor.byteOffset + colorView.byteOffset]));
				}

				if (primitive.attributes.find("TANGENT") != primitive.attributes.end())
				{
					const tinygltf::Accessor &tangentAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
					const tinygltf::BufferView &tangentView = model.bufferViews[tangentAccessor.bufferView];
					bufferTangents = reinterpret_cast<const float *>(&(model.buffers[tangentView.buffer].data[tangentAccessor.byteOffset + tangentView.byteOffset]));
				}

				// Skinning
				// Joints
				if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end()) {
					const tinygltf::Accessor &jointAccessor = model.accessors[primitive.attributes.find("JOINTS_0")->second];
					const tinygltf::BufferView &jointView = model.bufferViews[jointAccessor.bufferView];
					bufferJoints = reinterpret_cast<const uint16_t *>(&(model.buffers[jointView.buffer].data[jointAccessor.byteOffset + jointView.byteOffset]));
				}

				if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {
					const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("WEIGHTS_0")->second];
					const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
					bufferWeights = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
				}

				hasSkin = (bufferJoints && bufferWeights);

				vertexCount = static_cast<uint32_t>(posAccessor.count);

				for (size_t v = 0; v < posAccessor.count; v++) {
					Vertex vert{};
					vert.pos = glm::vec4(glm::make_vec3(&bufferPos[v * 3]), 1.0f);
					vert.normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
					vert.uv = bufferTexCoords ? glm::make_vec2(&bufferTexCoords[v * 2]) : glm::vec3(0.0f);
					if (bufferColors) {
						switch (numColorComponents) {
							case 3: 
								vert.color = glm::vec4(glm::make_vec3(&bufferColors[v * 3]), 1.0f);
								break;
							case 4:
								vert.color = glm::make_vec4(&bufferColors[v * 4]);
								break;
						}
					}
					else {
						vert.color = glm::vec4(1.0f);
					}
					vert.tangent = bufferTangents ? glm::vec4(glm::make_vec4(&bufferTangents[v * 4])) : glm::vec4(0.0f);
					vert.joint0 = hasSkin ? glm::vec4(glm::make_vec4(&bufferJoints[v * 4])) : glm::vec4(0.0f);
					vert.weight0 = hasSkin ? glm::make_vec4(&bufferWeights[v * 4]) : glm::vec4(0.0f);
					vertexBuffer.push_back(vert);
				}
			}
			// Indices
			{
				const tinygltf::Accessor &accessor = model.accessors[primitive.indices];
				const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
				const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];

				indexCount = static_cast<uint32_t>(accessor.count);

				switch (accessor.componentType) {
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
					uint32_t *buf = new uint32_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint32_t));
					for (size_t index = 0; index < accessor.count; index++) {
						indexBuffer.push_back(buf[index] + vertexStart);
					}
                    delete[] buf;
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
					uint16_t *buf = new uint16_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint16_t));
					for (size_t index = 0; index < accessor.count; index++) {
						indexBuffer.push_back(buf[index] + vertexStart);
					}
                    delete[] buf;
                    break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
					uint8_t *buf = new uint8_t[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(uint8_t));
					for (size_t index = 0; index < accessor.count; index++) {
						indexBuffer.push_back(buf[index] + vertexStart);
					}
                    delete[] buf;
                    break;
				}
				default:
					std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
					return;
				}
			}
			Primitive *newPrimitive = new Primitive(indexStart, indexCount, primitive.material > -1 ? materials[primitive.material] : materials.back());
			newPrimitive->firstVertex = vertexStart;
			newPrimitive->vertexCount = vertexCount;
			newPrimitive->setDimensions(posMin, posMax);
			newMesh->primitives.push_back(newPrimitive);
		}
		newNode->mesh = newMesh;
	}
	if (parent) {
		parent->children.push_back(newNode);
	} else {
		nodes.push_back(newNode);
	}
	linearNodes.push_back(newNode);
}

void vkglTF::Model::loadSkins(tinygltf::Model &gltfModel)
{
	for (tinygltf::Skin &source : gltfModel.skins) {
		Skin *newSkin = new Skin{};
		newSkin->name = source.name;
				
		// Find skeleton root node
		if (source.skeleton > -1) {
			newSkin->skeletonRoot = nodeFromIndex(source.skeleton);
		}

		// Find joint nodes
		for (int jointIndex : source.joints) {
			Node* node = nodeFromIndex(jointIndex);
			if (node) {
				newSkin->joints.push_back(nodeFromIndex(jointIndex));
			}
		}

		// Get inverse bind matrices from buffer
		if (source.inverseBindMatrices > -1) {
			const tinygltf::Accessor &accessor = gltfModel.accessors[source.inverseBindMatrices];
			const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
			const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];
			newSkin->inverseBindMatrices.resize(accessor.count);
			memcpy(newSkin->inverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::mat4));
		}

		skins.push_back(newSkin);
	}
}

void vkglTF::Model::loadImages(tinygltf::Model &gltfModel, AptnDevice device, AptnQueue transferQueue)
{
	for (tinygltf::Image &image : gltfModel.images) {
		vkglTF::Texture texture;
		texture.fromglTfImage(image, path, device, transferQueue);
		texture.index = static_cast<uint32_t>(textures.size());
		textures.push_back(texture);
	}
	// Create an empty texture to be used for empty material images
	createEmptyTexture(transferQueue);
}

void vkglTF::Model::loadMaterials(tinygltf::Model &gltfModel)
{
	for (tinygltf::Material &mat : gltfModel.materials) {
		vkglTF::Material material(device);
		if (mat.values.find("baseColorTexture") != mat.values.end()) {
			material.baseColorTexture = getTexture(gltfModel.textures[mat.values["baseColorTexture"].TextureIndex()].source);
		}
		// Metallic roughness workflow
		if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
			material.metallicRoughnessTexture = getTexture(gltfModel.textures[mat.values["metallicRoughnessTexture"].TextureIndex()].source);
		}
		if (mat.values.find("roughnessFactor") != mat.values.end()) {
			material.roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
		}
		if (mat.values.find("metallicFactor") != mat.values.end()) {
			material.metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
		}
		if (mat.values.find("baseColorFactor") != mat.values.end()) {
			material.baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
		}				
		if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
			material.normalTexture = getTexture(gltfModel.textures[mat.additionalValues["normalTexture"].TextureIndex()].source);
		} else {
			material.normalTexture = &emptyTexture;
		}
		if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
			material.emissiveTexture = getTexture(gltfModel.textures[mat.additionalValues["emissiveTexture"].TextureIndex()].source);
		}
		if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
			material.occlusionTexture = getTexture(gltfModel.textures[mat.additionalValues["occlusionTexture"].TextureIndex()].source);
		}
		if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
			tinygltf::Parameter param = mat.additionalValues["alphaMode"];
			if (param.string_value == "BLEND") {
				material.alphaMode = Material::ALPHAMODE_BLEND;
			}
			if (param.string_value == "MASK") {
				material.alphaMode = Material::ALPHAMODE_MASK;
			}
		}
		if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
			material.alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
		}

		materials.push_back(material);
	}
	// Push a default material at the end of the list for meshes with no material assigned
	materials.push_back(Material(device));
}

void vkglTF::Model::loadAnimations(tinygltf::Model &gltfModel)
{
	for (tinygltf::Animation &anim : gltfModel.animations) {
		vkglTF::Animation animation{};
		animation.name = anim.name;
		if (anim.name.empty()) {
			animation.name = std::to_string(animations.size());
		}

		// Samplers
		for (auto &samp : anim.samplers) {
			vkglTF::AnimationSampler sampler{};

			if (samp.interpolation == "LINEAR") {
				sampler.interpolation = AnimationSampler::InterpolationType::LINEAR;
			}
			if (samp.interpolation == "STEP") {
				sampler.interpolation = AnimationSampler::InterpolationType::STEP;
			}
			if (samp.interpolation == "CUBICSPLINE") {
				sampler.interpolation = AnimationSampler::InterpolationType::CUBICSPLINE;
			}

			// Read sampler input time values
			{
				const tinygltf::Accessor &accessor = gltfModel.accessors[samp.input];
				const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				float *buf = new float[accessor.count];
				memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(float));
				for (size_t index = 0; index < accessor.count; index++) {
					sampler.inputs.push_back(buf[index]);
				}
                delete[] buf;
				for (auto input : sampler.inputs) {
					if (input < animation.start) {
						animation.start = input;
					};
					if (input > animation.end) {
						animation.end = input;
					}
				}
			}

			// Read sampler output T/R/S values 
			{
				const tinygltf::Accessor &accessor = gltfModel.accessors[samp.output];
				const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];

				assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

				switch (accessor.type) {
				case TINYGLTF_TYPE_VEC3: {
					glm::vec3 *buf = new glm::vec3[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::vec3));
					for (size_t index = 0; index < accessor.count; index++) {
						sampler.outputsVec4.push_back(glm::vec4(buf[index], 0.0f));
					}
                    delete[] buf;
                    break;
				}
				case TINYGLTF_TYPE_VEC4: {
					glm::vec4 *buf = new glm::vec4[accessor.count];
					memcpy(buf, &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::vec4));
					for (size_t index = 0; index < accessor.count; index++) {
						sampler.outputsVec4.push_back(buf[index]);
					}
                    delete[] buf;
                    break;
				}
				default: {
					std::cout << "unknown type" << std::endl;
					break;
				}
				}
			}

			animation.samplers.push_back(sampler);
		}

		// Channels
		for (auto &source: anim.channels) {
			vkglTF::AnimationChannel channel{};

			if (source.target_path == "rotation") {
				channel.path = AnimationChannel::PathType::ROTATION;
			}
			if (source.target_path == "translation") {
				channel.path = AnimationChannel::PathType::TRANSLATION;
			}
			if (source.target_path == "scale") {
				channel.path = AnimationChannel::PathType::SCALE;
			}
			if (source.target_path == "weights") {
				std::cout << "weights not yet supported, skipping channel" << std::endl;
				continue;
			}
			channel.samplerIndex = source.sampler;
			channel.node = nodeFromIndex(source.target_node);
			if (!channel.node) {
				continue;
			}

			animation.channels.push_back(channel);
		}

		animations.push_back(animation);
	}
}

void vkglTF::Model::loadFromFile(std::string filename, AptnDevice device, AptnQueue transferQueue, uint32_t fileLoadingFlags, float scale)
{
	tinygltf::Model gltfModel;
	tinygltf::TinyGLTF gltfContext;
	if (fileLoadingFlags & FileLoadingFlags::DontLoadImages) {
		gltfContext.SetImageLoader(loadImageDataFuncEmpty, nullptr);
	} else {
		gltfContext.SetImageLoader(loadImageDataFunc, nullptr);
	}
#if defined(__ANDROID__)
	// On Android all assets are packed with the apk in a compressed form, so we need to open them using the asset manager
	// We let tinygltf handle this, by passing the asset manager of our app
	tinygltf::asset_manager = androidApp->activity->assetManager;
#endif
	size_t pos = filename.find_last_of('/');
	path = filename.substr(0, pos);

	std::string error, warning;

	this->device = device;

#if defined(__ANDROID__)
	// On Android all assets are packed with the apk in a compressed form, so we need to open them using the asset manager
	// We let tinygltf handle this, by passing the asset manager of our app
	tinygltf::asset_manager = androidApp->activity->assetManager;
#endif
	bool fileLoaded = gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filename);

	std::vector<uint32_t> indexBuffer;
	std::vector<Vertex> vertexBuffer;

	if (fileLoaded) {
		if (!(fileLoadingFlags & FileLoadingFlags::DontLoadImages)) {
			loadImages(gltfModel, device, transferQueue);
		}
		loadMaterials(gltfModel);
		const tinygltf::Scene &scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
		for (size_t i = 0; i < scene.nodes.size(); i++) {
			const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
			loadNode(nullptr, node, scene.nodes[i], gltfModel, indexBuffer, vertexBuffer, scale);
		}
		if (gltfModel.animations.size() > 0) {
			loadAnimations(gltfModel);
		}
		loadSkins(gltfModel);

		for (auto node : linearNodes) {
			// Assign skins
			if (node->skinIndex > -1) {
				node->skin = skins[node->skinIndex];
			}
			// Initial pose
			if (node->mesh) {
				node->update();
			}
		}
	}
	else {
		Assert(false);
		//vks::tools::exitFatal("Could not load glTF file \"" + filename + "\": " + error, -1);
		return;
	}

	// Pre-Calculations for requested features
	if ((fileLoadingFlags & FileLoadingFlags::PreTransformVertices) || (fileLoadingFlags & FileLoadingFlags::PreMultiplyVertexColors) || (fileLoadingFlags & FileLoadingFlags::FlipY)) {
		const bool preTransform = fileLoadingFlags & FileLoadingFlags::PreTransformVertices;
		const bool preMultiplyColor = fileLoadingFlags & FileLoadingFlags::PreMultiplyVertexColors;
		const bool flipY = fileLoadingFlags & FileLoadingFlags::FlipY;
		const bool flipUV = fileLoadingFlags & FileLoadingFlags::FlipUV;
		for (Node* node : linearNodes) {
			if (node->mesh) {
				const glm::mat4 localMatrix = node->getMatrix();
				for (Primitive* primitive : node->mesh->primitives) {
					for (uint32_t i = 0; i < primitive->vertexCount; i++) {
						Vertex& vertex = vertexBuffer[primitive->firstVertex + i];
						// Pre-transform vertex positions by node-hierarchy
						if (preTransform) {
							vertex.pos = glm::vec3(localMatrix * glm::vec4(vertex.pos, 1.0f));
							vertex.normal = glm::normalize(glm::mat3(localMatrix) * vertex.normal);
						}
						// Flip Y-Axis of vertex positions
						if (flipY) {
							vertex.pos.y *= -1.0f;
							vertex.normal.y *= -1.0f;
						}
						// Flip textures verticall
						if (flipUV) {
							vertex.uv.t = 1.0f - vertex.uv.t;
						}
						// Pre-Multiply vertex colors with material base color
						if (preMultiplyColor) {
							vertex.color = primitive->material.baseColorFactor * vertex.color;
						}
					}
				}
			}
		}
	}

	for (auto& extension : gltfModel.extensionsUsed) {
		if (extension == "KHR_materials_pbrSpecularGlossiness") {
			std::cout << "Required extension: " << extension;
			metallicRoughnessWorkflow = false;
		}
	}

	size_t vertexBufferSize = vertexBuffer.size() * sizeof(Vertex);
	size_t indexBufferSize = indexBuffer.size() * sizeof(uint32_t);
	indices.count = static_cast<uint32_t>(indexBuffer.size());
	vertices.count = static_cast<uint32_t>(vertexBuffer.size());

	assert((vertexBufferSize > 0) && (indexBufferSize > 0));

	// Vertex data
	AptnBufferCreationParams bufferParams
	{
		.usage = AptnBufferUsageFlags::TransferSrc,
		.size = vertexBufferSize,
		.supportsMappedMemory = true
	};

	// Create staging buffers
	// Vertex data
	AptnBuffer vertexStaging = Apparition::CreateBuffer(device, bufferParams);
	{
		void* data = Apparition::MapBuffer(vertexStaging);
		Memcpy(data, vertexBuffer.data(), vertexBufferSize);
		Apparition::UnmapBuffer(vertexStaging);
	}

	// Index data
	bufferParams.size = indexBufferSize;
	AptnBuffer indexStaging = Apparition::CreateBuffer(device, bufferParams);
	{
		void* data = Apparition::MapBuffer(indexStaging);
		Memcpy(data, indexBuffer.data(), indexBufferSize);
		Apparition::UnmapBuffer(indexStaging);
	}

	// Create device local buffers
	// Vertex buffer
	bufferParams.supportsMappedMemory = false;
	bufferParams.size = vertexBufferSize;
	bufferParams.usage = AptnBufferUsageFlags::VertexBuffer | AptnBufferUsageFlags::TransferDst; // | optional ray tracing flags
	vertices.buffer = Apparition::CreateBuffer(device, bufferParams);

	// Index buffer
	bufferParams.size = indexBufferSize;
	bufferParams.usage = AptnBufferUsageFlags::IndexBuffer | AptnBufferUsageFlags::TransferDst; // | optional ray tracing flags
	indices.buffer = Apparition::CreateBuffer(device, bufferParams);

	// Copy from staging buffers
	AptnCommandPoolCreationParams params
	{
		.queueIndex = Apparition::GetQueueIndex(transferQueue)
	};
	AptnCommandPool commandPool = Apparition::CreateCommandPool(device, params);
	AptnCommandBufferAllocParams cmdBuffParams;
	AptnCommandBuffer copyCmd = Apparition::AllocateCommandBuffer(commandPool, cmdBuffParams);
	Apparition::BeginCommandBuffer(copyCmd);

	AptnBufferCopyDesc copyDesc
	{
		.size = vertexBufferSize,
		.srcBuffer = vertexStaging,
		.dstBuffer = vertices.buffer
	};

	Apparition::CopyBuffer(copyCmd, copyDesc);

	copyDesc.size = indexBufferSize;
	copyDesc.srcBuffer = indexStaging;
	copyDesc.dstBuffer = indices.buffer;
	Apparition::CopyBuffer(copyCmd, copyDesc);

	Apparition::EndCommandBuffer(copyCmd);
	Apparition::SubmitCommandBuffer(transferQueue, copyCmd);
	Apparition::WaitForIdle(transferQueue);
	Apparition::FreeCommandBuffer(copyCmd);
	Apparition::DestroyCommandPool(commandPool);

	Apparition::DestroyBuffer(vertexStaging);
	Apparition::DestroyBuffer(indexStaging);

	getSceneDimensions();

	// Setup descriptors
	uint32_t uboCount{ 0 };
	uint32_t imageCount{ 0 };
	for (auto& node : linearNodes) {
		if (node->mesh) {
			uboCount++;
		}
	}
	for (auto& material : materials) {
		if (material.baseColorTexture != nullptr) {
			imageCount++;
		}
	}
	
	AptnDescriptorPoolCreationParams poolParams
	{
		.poolSizes
		{
			AptnDescriptorPoolSize
			{
				.poolType = AptnDescriptor::UniformBuffer,
				.size = uboCount
			}
		}
	};
	auto& poolSizes = poolParams.poolSizes;
	if (imageCount > 0) {
		if (descriptorBindingFlags & DescriptorBindingFlags::ImageBaseColor) {
			poolSizes.Add(
				AptnDescriptorPoolSize
				{
					.poolType = AptnDescriptor::CombinedImageSampler,
					.size = imageCount
				}
			);
		}
		if (descriptorBindingFlags & DescriptorBindingFlags::ImageNormalMap) {
			poolSizes.Add(
				AptnDescriptorPoolSize
				{
					.poolType = AptnDescriptor::CombinedImageSampler,
					.size = imageCount
				}
			);
		}
	}

	descriptorPool = Apparition::CreateDescriptorPool(device, poolParams);

	// Descriptors for per-node uniform buffers
	{
		// Layout is global, so only create if it hasn't already been created before
		if (!IsValid(descriptorSetLayoutUbo)) {
			AptnDescriptorSetLayoutCreationParams params
			{
				.bindings = 
				{
					AptnDescriptorSetLayoutDesc
					{
						.binding = 0,
						.descriptorType = AptnDescriptor::UniformBuffer,
						.descriptorCount = 1,
						.shaderStageFlags = AptnShaderStageFlags::Vertex
					}
				}
			};
			descriptorSetLayoutUbo = Apparition::CreateDescriptorSetLayout(device, params);
		}
		for (auto node : nodes) {
			prepareNodeDescriptor(node, descriptorSetLayoutUbo);
		}
	}

	// Descriptors for per-material images
	{
		// Layout is global, so only create if it hasn't already been created before
		if (!IsValid(descriptorSetLayoutImage)) {
			AptnDescriptorSetLayoutCreationParams params;
			if (descriptorBindingFlags & DescriptorBindingFlags::ImageBaseColor) 
			{
				params.bindings.Add(
					AptnDescriptorSetLayoutDesc
					{
						.binding = params.bindings.Size(),
						.descriptorType = AptnDescriptor::CombinedImageSampler,
						.descriptorCount = 1,
						.shaderStageFlags = AptnShaderStageFlags::Fragment
					});
			}
			if (descriptorBindingFlags & DescriptorBindingFlags::ImageNormalMap)
			{
				params.bindings.Add(
					AptnDescriptorSetLayoutDesc
					{ 
						.binding = params.bindings.Size(),
						.descriptorType = AptnDescriptor::CombinedImageSampler,
						.descriptorCount = 1,
						.shaderStageFlags = AptnShaderStageFlags::Fragment
					});
			}

			descriptorSetLayoutImage = Apparition::CreateDescriptorSetLayout(device, params);
		}
		for (auto& material : materials) {
			if (material.baseColorTexture != nullptr) {
				material.createDescriptorSet(descriptorPool, vkglTF::descriptorSetLayoutImage, descriptorBindingFlags);
			}
		}
	}
}

void vkglTF::Model::bindBuffers(AptnCommandBuffer commandBuffer)
{
	AptnBindVertexBufferDesc vertBind
	{
		.vertexBuffer = vertices.buffer
	};
	AptnBindIndexBufferDesc idxBind
	{
		.indexBuffer = indices.buffer
	};
	Apparition::BindVertexBuffers(commandBuffer, vertBind);
	Apparition::BindIndexBuffer(commandBuffer, idxBind);
	buffersBound = true;
}

void vkglTF::Model::drawNode(Node *node, AptnCommandBuffer commandBuffer, uint32_t renderFlags, const AptnPipelineDescription& pipelineDesc, uint32_t bindImageSet)
{
	if (node->mesh) {
		for (Primitive* primitive : node->mesh->primitives) {
			bool skip = false;
			const vkglTF::Material& material = primitive->material;
			if (renderFlags & RenderFlags::RenderOpaqueNodes) {
				skip = (material.alphaMode != Material::ALPHAMODE_OPAQUE);
			}
			if (renderFlags & RenderFlags::RenderAlphaMaskedNodes) {
				skip = (material.alphaMode != Material::ALPHAMODE_MASK);
			}
			if (renderFlags & RenderFlags::RenderAlphaBlendedNodes) {
				skip = (material.alphaMode != Material::ALPHAMODE_BLEND);
			}
			if (!skip) {
				if (renderFlags & RenderFlags::BindImages) {
					AptnBindDescriptorSetsDesc descriptorSetsDesc
					{
						.pipelineDesc = pipelineDesc,
						.bindPoint = AptnBindPoint::Graphics,
						.firstSet = 0,
						.descriptorSets = { material.descriptorSet }
					};
					Apparition::BindDescriptorSets(commandBuffer, descriptorSetsDesc);
				}
				Apparition::DrawIndexed(commandBuffer, primitive->indexCount, primitive->firstIndex);
			}
		}
	}
	for (auto& child : node->children) {
		drawNode(child, commandBuffer, renderFlags, pipelineDesc, bindImageSet);
	}
}

void vkglTF::Model::draw(AptnCommandBuffer commandBuffer, uint32_t renderFlags, const AptnPipelineDescription& pipelineDesc, uint32_t bindImageSet)
{
	if (!buffersBound) {
		AptnBindVertexBufferDesc vertBind
		{
			.vertexBuffer = vertices.buffer
		};
		AptnBindIndexBufferDesc idxBind
		{
			.indexBuffer = indices.buffer
		};
		Apparition::BindVertexBuffers(commandBuffer, vertBind);
		Apparition::BindIndexBuffer(commandBuffer, idxBind);
	}
	for (auto& node : nodes) {
		drawNode(node, commandBuffer, renderFlags, pipelineDesc, bindImageSet);
	}
}

void vkglTF::Model::getNodeDimensions(Node *node, glm::vec3 &min, glm::vec3 &max)
{
	if (node->mesh) {
		for (Primitive *primitive : node->mesh->primitives) {
			glm::vec4 locMin = glm::vec4(primitive->dimensions.min, 1.0f) * node->getMatrix();
			glm::vec4 locMax = glm::vec4(primitive->dimensions.max, 1.0f) * node->getMatrix();
			if (locMin.x < min.x) { min.x = locMin.x; }
			if (locMin.y < min.y) { min.y = locMin.y; }
			if (locMin.z < min.z) { min.z = locMin.z; }
			if (locMax.x > max.x) { max.x = locMax.x; }
			if (locMax.y > max.y) { max.y = locMax.y; }
			if (locMax.z > max.z) { max.z = locMax.z; }
		}
	}
	for (auto child : node->children) {
		getNodeDimensions(child, min, max);
	}
}

void vkglTF::Model::getSceneDimensions()
{
	dimensions.min = glm::vec3(FLT_MAX);
	dimensions.max = glm::vec3(-FLT_MAX);
	for (auto node : nodes) {
		getNodeDimensions(node, dimensions.min, dimensions.max);
	}
	dimensions.size = dimensions.max - dimensions.min;
	dimensions.center = (dimensions.min + dimensions.max) / 2.0f;
	dimensions.radius = glm::distance(dimensions.min, dimensions.max) / 2.0f;
}

void vkglTF::Model::updateAnimation(uint32_t index, float time)
{
	if (index > static_cast<uint32_t>(animations.size()) - 1) {
		std::cout << "No animation with index " << index << std::endl;
		return;
	}
	Animation &animation = animations[index];

	bool updated = false;
	for (auto& channel : animation.channels) {
		vkglTF::AnimationSampler &sampler = animation.samplers[channel.samplerIndex];
		if (sampler.inputs.size() > sampler.outputsVec4.size()) {
			continue;
		}

		for (size_t i = 0; i < sampler.inputs.size() - 1; i++) {
			if ((time >= sampler.inputs[i]) && (time <= sampler.inputs[i + 1])) {
				float u = std::max(0.0f, time - sampler.inputs[i]) / (sampler.inputs[i + 1] - sampler.inputs[i]);
				if (u <= 1.0f) {
					switch (channel.path) {
					case vkglTF::AnimationChannel::PathType::TRANSLATION: {
						glm::vec4 trans = glm::mix(sampler.outputsVec4[i], sampler.outputsVec4[i + 1], u);
						channel.node->translation = glm::vec3(trans);
						break;
					}
					case vkglTF::AnimationChannel::PathType::SCALE: {
						glm::vec4 trans = glm::mix(sampler.outputsVec4[i], sampler.outputsVec4[i + 1], u);
						channel.node->scale = glm::vec3(trans);
						break;
					}
					case vkglTF::AnimationChannel::PathType::ROTATION: {
						glm::quat q1;
						q1.x = sampler.outputsVec4[i].x;
						q1.y = sampler.outputsVec4[i].y;
						q1.z = sampler.outputsVec4[i].z;
						q1.w = sampler.outputsVec4[i].w;
						glm::quat q2;
						q2.x = sampler.outputsVec4[i + 1].x;
						q2.y = sampler.outputsVec4[i + 1].y;
						q2.z = sampler.outputsVec4[i + 1].z;
						q2.w = sampler.outputsVec4[i + 1].w;
						channel.node->rotation = glm::normalize(glm::slerp(q1, q2, u));
						break;
					}
					}
					updated = true;
				}
			}
		}
	}
	if (updated) {
		for (auto &node : nodes) {
			node->update();
		}
	}
}

/*
	Helper functions
*/
vkglTF::Node* vkglTF::Model::findNode(Node *parent, uint32_t index) {
	Node* nodeFound = nullptr;
	if (parent->index == index) {
		return parent;
	}
	for (auto& child : parent->children) {
		nodeFound = findNode(child, index);
		if (nodeFound) {
			break;
		}
	}
	return nodeFound;
}

vkglTF::Node* vkglTF::Model::nodeFromIndex(uint32_t index) {
	Node* nodeFound = nullptr;
	for (auto &node : nodes) {
		nodeFound = findNode(node, index);
		if (nodeFound) {
			break;
		}
	}
	return nodeFound;
}

vkglTF::Node* vkglTF::Model::nodeFromName(const std::string name) {
	//Node* nodeFound = nullptr;
	for (auto& node : linearNodes) {
		if (node->name == name) {
			return node;
		}
	}
	return nullptr;
}

void vkglTF::Model::prepareNodeDescriptor(vkglTF::Node* node, AptnDescriptorSetLayout descriptorSetLayout) {
	if (node->mesh) {
		AptnDescriptorSetAllocParams dsAllocParams
		{
			.layout = descriptorSetLayout
		};
		node->mesh->uniformBuffer.descriptorSet = Apparition::AllocateDescriptorSet(descriptorPool, dsAllocParams);

		AptnUpdateDescriptorSetDesc updateDesc
		{
			.descriptorSet = node->mesh->uniformBuffer.descriptorSet,
			.setBinding = 0,
			.descriptorType = AptnDescriptor::UniformBuffer,
			.bufferDescriptor = &node->mesh->uniformBuffer.descriptor
		};
		Apparition::UpdateDescriptorSets({ updateDesc });
	}
	for (auto& child : node->children) {
		prepareNodeDescriptor(child, descriptorSetLayout);
	}
}
