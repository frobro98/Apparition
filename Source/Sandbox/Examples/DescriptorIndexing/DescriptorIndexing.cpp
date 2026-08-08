
#include "DescriptorIndexing.h"

#include "Math/Matrix4.hpp"
#include "Math/Vector3.hpp"
#include "Math/Vector2.hpp"

#include "glTFModel/VulkanglTFModel.h"
#include "glTFModel/VulkanTexture.h"

#include <random>

namespace
{
// Dynamically indexed textures
DynamicArray<vks::Texture2D> textures;

AptnBuffer vertexBuffer;
AptnBuffer indexBuffer;
u32 indexCount = 0;

struct UniformData
{
    Matrix4 projection;
    Matrix4 view;
    Matrix4 model;
} uniformData;
StaticArray<AptnBuffer, maxConcurrentFrames> uniformBuffers;

AptnDevice device;
AptnQueue graphicsQueue;

AptnPipeline pipeline;

AptnDescriptorPool descriptorPool;
AptnDescriptorSetLayout descriptorSetLayout;
StaticArray<AptnDescriptorSet, maxConcurrentFrames> descriptorSets;

AptnCommandPool commandPool;
StaticArray<AptnCommandBuffer, maxConcurrentFrames> drawCommandBuffers;
AptnImage depthStencilImage;
AptnImageView depthStencilView;

struct Vertex
{
    Vector3 pos;
    Vector2 uv;
    i32 textureIndex;
};
}

void generateCubeTextures()
{
    constexpr u32 textureCount = 32;
    textures.Resize(textureCount);
    for (i32 i = 0; i < textures.Size(); ++i)
    {
        std::random_device rndDevice;
        std::default_random_engine rndEngine(rndDevice());
        std::uniform_int_distribution<> rndDist(50, UCHAR_MAX);
        const int32_t dim = 3;
        const size_t bufferSize = dim * dim * 4;
        DynamicArray<u8> textureData(bufferSize);
        for (i32 j = 0; j < dim * dim; ++j)
        {
            textureData[j * 4] = rndDist(rndEngine);
            textureData[j * 4 + 1] = rndDist(rndEngine);
            textureData[j * 4 + 2] = rndDist(rndEngine);
            textureData[j * 4 + 3] = 255;
        }
        textures[i].fromBuffer(textureData.GetData(), bufferSize, AptnImageFormat::RGB_8norm, dim, dim, device, graphicsQueue, AptnSamplerFilter::Nearest);
    }
}

void generateCubes()
{
	DynamicArray<Vertex> vertices;
	DynamicArray<uint32_t> indices;

	// Generate random per-face texture indices
	std::random_device rndDevice;
	std::default_random_engine rndEngine(rndDevice());
	std::uniform_int_distribution<int32_t> rndDist(0, static_cast<uint32_t>(textures.Size()) - 1);

	// Generate cubes with random per-face texture indices
	const uint32_t count = 5;
	for (uint32_t i = 0; i < count; i++) {
		// Push indices to buffer
		const DynamicArray<u32> cubeIndices = {
			0,1,2,0,2,3,
			4,5,6,4,6,7,
			8,9,10,8,10,11,
			12,13,14,12,14,15,
			16,17,18,16,18,19,
			20,21,22,20,22,23
		};
		const u32 indexOffset = vertices.Size();
		for (auto& index : cubeIndices) {
			indices.Add(index + indexOffset);
		}
		// Get random per-Face texture indices that the shader will sample from
		i32 textureIndices[6];
		for (u32 j = 0; j < 6; j++) {
			textureIndices[j] = rndDist(rndEngine);
		}
		// Push vertices to buffer
		f32 pos = 2.5f * i - (count * 2.5f / 2.0f) + 1.25f;
		const DynamicArray<Vertex> cube = {
			Vertex{ .pos{ -1.0f + pos, -1.0f,  1.0f }, .uv{ 0.0f, 0.0f }, .textureIndex = textureIndices[0] },
			Vertex{ .pos{  1.0f + pos, -1.0f,  1.0f }, .uv{ 1.0f, 0.0f }, .textureIndex = textureIndices[0] },
			Vertex{ .pos{  1.0f + pos,  1.0f,  1.0f }, .uv{ 1.0f, 1.0f }, .textureIndex = textureIndices[0] },
			Vertex{ .pos{ -1.0f + pos,  1.0f,  1.0f }, .uv{ 0.0f, 1.0f }, .textureIndex = textureIndices[0] },

			Vertex{ .pos{  1.0f + pos,  1.0f,  1.0f }, .uv{ 0.0f, 0.0f }, .textureIndex = textureIndices[1] },
			Vertex{ .pos{  1.0f + pos,  1.0f, -1.0f }, .uv{ 1.0f, 0.0f }, .textureIndex = textureIndices[1] },
			Vertex{ .pos{  1.0f + pos, -1.0f, -1.0f }, .uv{ 1.0f, 1.0f }, .textureIndex = textureIndices[1] },
			Vertex{ .pos{  1.0f + pos, -1.0f,  1.0f }, .uv{ 0.0f, 1.0f }, .textureIndex = textureIndices[1] },

			Vertex{ .pos{ -1.0f + pos, -1.0f, -1.0f }, .uv{ 0.0f, 0.0f }, .textureIndex = textureIndices[2] },
			Vertex{ .pos{  1.0f + pos, -1.0f, -1.0f }, .uv{ 1.0f, 0.0f }, .textureIndex = textureIndices[2] },
			Vertex{ .pos{  1.0f + pos,  1.0f, -1.0f }, .uv{ 1.0f, 1.0f }, .textureIndex = textureIndices[2] },
			Vertex{ .pos{ -1.0f + pos,  1.0f, -1.0f }, .uv{ 0.0f, 1.0f }, .textureIndex = textureIndices[2] },

			Vertex{ .pos{ -1.0f + pos, -1.0f, -1.0f }, .uv{ 0.0f, 0.0f }, .textureIndex = textureIndices[3] },
			Vertex{ .pos{ -1.0f + pos, -1.0f,  1.0f }, .uv{ 1.0f, 0.0f }, .textureIndex = textureIndices[3] },
			Vertex{ .pos{ -1.0f + pos,  1.0f,  1.0f }, .uv{ 1.0f, 1.0f }, .textureIndex = textureIndices[3] },
			Vertex{ .pos{ -1.0f + pos,  1.0f, -1.0f }, .uv{ 0.0f, 1.0f }, .textureIndex = textureIndices[3] },

			Vertex{ .pos{  1.0f + pos,  1.0f,  1.0f }, .uv{ 0.0f, 0.0f }, .textureIndex = textureIndices[4] },
			Vertex{ .pos{ -1.0f + pos,  1.0f,  1.0f }, .uv{ 1.0f, 0.0f }, .textureIndex = textureIndices[4] },
			Vertex{ .pos{ -1.0f + pos,  1.0f, -1.0f }, .uv{ 1.0f, 1.0f }, .textureIndex = textureIndices[4] },
			Vertex{ .pos{  1.0f + pos,  1.0f, -1.0f }, .uv{ 0.0f, 1.0f }, .textureIndex = textureIndices[4] },

			Vertex{ .pos{ -1.0f + pos, -1.0f, -1.0f }, .uv{ 0.0f, 0.0f }, .textureIndex = textureIndices[5] },
			Vertex{ .pos{  1.0f + pos, -1.0f, -1.0f }, .uv{ 1.0f, 0.0f }, .textureIndex = textureIndices[5] },
			Vertex{ .pos{  1.0f + pos, -1.0f,  1.0f }, .uv{ 1.0f, 1.0f }, .textureIndex = textureIndices[5] },
			Vertex{ .pos{ -1.0f + pos, -1.0f,  1.0f }, .uv{ 0.0f, 1.0f }, .textureIndex = textureIndices[5] },
		};
		for (auto& vertex : cube) {
			vertices.Add(vertex);
		}
	}

	indexCount = indices.Size();

	AptnBufferCreationParams bufferParams
	{
		.size = vertices.SizeInBytes(),
		.supportsMappedMemory = true,
		.usage = AptnBufferUsageFlagBits::TransferSrc
	};
	AptnBuffer vertStaging = Apparition::CreateBuffer(device, bufferParams);
	{
		void* data = Apparition::MapBuffer(vertStaging);
		Memcpy(data, vertices.GetData(), vertices.SizeInBytes());
		Apparition::UnmapBuffer(vertStaging);
	}

	bufferParams.size = indices.SizeInBytes();
	AptnBuffer idxStaging = Apparition::CreateBuffer(device, bufferParams);
	{
		void* data = Apparition::MapBuffer(idxStaging);
		Memcpy(data, indices.GetData(), indices.SizeInBytes());
		Apparition::UnmapBuffer(idxStaging);
	}

	bufferParams.size = vertices.SizeInBytes();
	bufferParams.supportsMappedMemory = false;
	bufferParams.usage = AptnBufferUsageFlagBits::TransferDst | AptnBufferUsageFlagBits::VertexBuffer;
	vertexBuffer = Apparition::CreateBuffer(device, bufferParams);

	bufferParams.size = indices.SizeInBytes();
	bufferParams.supportsMappedMemory = false;
	bufferParams.usage = AptnBufferUsageFlagBits::TransferDst | AptnBufferUsageFlagBits::IndexBuffer;
	indexBuffer = Apparition::CreateBuffer(device, bufferParams);

	Assert(IsValid(commandPool));
	AptnCommandBuffer copyBuffer = Apparition::AllocateCommandBuffer(commandPool);

	{
		Apparition::BeginCommandBuffer(copyBuffer, true);

		{
			AptnBufferCopyDesc copyDesc
			{
				.srcBuffer = vertStaging,
				.dstBuffer = vertexBuffer,
				.size = vertices.SizeInBytes()
			};
			Apparition::CopyBuffer(copyBuffer, copyDesc);
		}
		{
			AptnBufferCopyDesc copyDesc
			{
				.srcBuffer = idxStaging,
				.dstBuffer = indexBuffer,
				.size = indices.SizeInBytes()
			};
			Apparition::CopyBuffer(copyBuffer, copyDesc);
		}

		Apparition::EndCommandBuffer(copyBuffer);
	}

	Apparition::SubmitCommandBuffer(graphicsQueue, copyBuffer);
	Apparition::WaitForIdle(graphicsQueue);
	Apparition::FreeCommandBuffer(copyBuffer);

	Apparition::DestroyBuffer(vertStaging);
	Apparition::DestroyBuffer(idxStaging);
}

void prepareUniformBuffers()
{
	for (auto& uniformBuffer : uniformBuffers)
	{
		AptnBufferCreationParams params
		{
			.size = sizeof(UniformData),
			.supportsMappedMemory = true,
			.usage = AptnBufferUsageFlagBits::UniformBuffer
		};
		uniformBuffer = Apparition::CreateBuffer(device, params);
	}
}

void setupDescriptors()
{
	AptnDescriptorPoolCreationParams poolParams
	{
		.poolSizes
		{
			AptnDescriptorPoolSize
			{
				.poolType = AptnDescriptor::UniformBuffer,
				.size = maxConcurrentFrames
			},
			AptnDescriptorPoolSize
			{
				.poolType = AptnDescriptor::CombinedImageSampler,
				.size = textures.Size() * maxConcurrentFrames
			}
		}
	};
	descriptorPool = Apparition::CreateDescriptorPool(device, poolParams);

	AptnDescriptorSetLayoutCreationParams layoutParams
	{
		.bindings
		{
			AptnDescriptorSetLayoutDesc
			{
				.binding = 0,
				.descriptorType = AptnDescriptor::UniformBuffer,
				.shaderStageFlags = AptnShaderStageFlagBits::Vertex
			},
			AptnDescriptorSetLayoutDesc
			{
				.binding = 1,
				.descriptorType = AptnDescriptor::CombinedImageSampler,
				.descriptorCount = textures.Size(),
				.shaderStageFlags = AptnShaderStageFlagBits::Fragment
			}
		}
	};
}

void InitializeDescriptorIndexingExample(AptnDevice inDevice)
{
    device = inDevice;
    graphicsQueue = Apparition::AllocateGraphicsQueue(device);

	AptnBackbufferSetupParams backbufferSetupParams{
        .wndHandle = window->windowHandle,
        .wndWidth = window->width,
        .wndHeight = window->height
    };
    Apparition::SetupBackbuffer(device, backbufferSetupParams);

	{
		AptnCommandPoolCreationParams createParams{
			.queueIndex = Apparition::GetQueueIndex(graphicsQueue),
			.canResetCommandBuffers = true
		};
		commandPool = Apparition::CreateCommandPool(device, createParams);
	}

	// Depth backbuffer
	{
		AptnImageCreationParams imageParams
		{
			.format = AptnImageFormat::DS_32f_8u,
			.mipLevels = 1,
			.width = window->width,
			.height = window->height,
			.usageFlags = AptnImageUsageFlagBits::DepthStencilAttachment
		};
		depthStencilImage = Apparition::CreateImage(device, imageParams);

		AptnImageViewCreationParams viewParams
		{
			.aspect = AptnImageAspect::DepthStencil,
			.baseMipLevel = 0,
			.format = AptnImageFormat::DS_32f_8u,
			.mipCount = 1
		};
		depthStencilView = Apparition::CreateImageView(depthStencilImage, viewParams);
	}

	// Draw Command Buffers
	{;
		drawCommandBuffers[0] = Apparition::AllocateCommandBuffer(commandPool);
		drawCommandBuffers[1] = Apparition::AllocateCommandBuffer(commandPool);
	}

	generateCubeTextures();
	generateCubes();
}

void TickDescriptorIndexingExample(/*Apparition::Device device*/)
{

}

void DestroyDescriptorIndexingExample(/*Apparition::Device device*/)
{

}
