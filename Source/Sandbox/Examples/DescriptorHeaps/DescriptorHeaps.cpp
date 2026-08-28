
#include "DescriptorHeaps.h"

#include "Apparition/DescriptorHeap.h"

#include "Math/MathFunctions.hpp"
#include "Math/Matrix4.hpp"
#include "Math/MatrixFunctions.hpp"
#include "Math/Quat.hpp"
#include "Math/Vector3.hpp"

#include "glTFModel/VulkanglTFModel.h"
#include "glTFModel/VulkanTexture.h"

extern f32 cameraRotationX;
extern f32 cameraRotationY;

namespace
{
StaticArray<vks::Texture2D, 2> textures{};

struct UniformData {
	Matrix4 mvp{ SCALE, 1.f };
	u32 samplerIndex{ 0 };
	u32 imageHeapIndexOffset{ 0 };
} uniformData;
StaticArray<AptnBuffer, maxConcurrentFrames> uniformBuffers;

// This per-model data will be accessed via resource heaps
struct ModelData {
	Vector4 pos;
	Vector4 color;
};
StaticArray<AptnBuffer, 2> modelDataBuffers;

struct PushConstantBlock {
	VkDeviceAddress matrixReference;
};

i32 selectedSampler{ 0 };
vkglTF::Model model;

AptnDevice device;
AptnQueue graphicsQueue;
AptnPipeline pipeline;
AptnCommandPool commandPool;

AptnResourceHeap resourceHeap;
AptnSamplerHeap samplerHeap;

StaticArray<AptnCommandBuffer, maxConcurrentFrames> drawCommandBuffers;
AptnImage depthStencilImage;
AptnImageView depthStencilView;
}


void InitializeDescriptorHeapsExample(AptnDevice inDevice)
{
	device = inDevice;
	graphicsQueue = Apparition::AllocateGraphicsQueue(device);

	AptnBackbufferSetupParams backbufferSetupParams{
		.wndHandle = window->windowHandle,
		.wndWidth = window->width,
		.wndHeight = window->height
	};
	Apparition::SetupBackbuffer(device, backbufferSetupParams);

	const uint32_t glTFLoadingFlags = vkglTF::FileLoadingFlags::PreTransformVertices | vkglTF::FileLoadingFlags::PreMultiplyVertexColors | vkglTF::FileLoadingFlags::FlipY;
	Path cubeModelPath = Path(SandboxModelPath()) / "cube.gltf";
	Path tex0Path = Path(SandboxTexturePath()) / "crate01_color_height_rgba.ktx";
	Path tex1Path = Path(SandboxTexturePath()) / "crate02_color_height_rgba.ktx";
	model.loadFromFile(cubeModelPath.GetString(), device, graphicsQueue, glTFLoadingFlags);
	textures[0].loadFromFile(tex0Path.GetString(), AptnImageFormat::RGBA_8norm, device, graphicsQueue);
	textures[1].loadFromFile(tex1Path.GetString(), AptnImageFormat::RGBA_8norm, device, graphicsQueue);

	for (AptnBuffer& buffer : uniformBuffers)
	{
		AptnBufferCreationParams params
		{
			.usage = AptnBufferUsageFlags::ShaderDeviceAddress,
			.size = sizeof(UniformData),
			.supportsMappedMemory = true
		};
		buffer = Apparition::CreateBuffer(device, params);
	}

	// Sampler Heap
	AptnSamplerHeapCreationParams samplerHeapParams
	{
		.samplerCount = 2
	};
	samplerHeap = Apparition::CreateSamplerHeap(device, samplerHeapParams);

	AptnSamplerDescriptor samplerDescriptor
	{
		.filter = AptnSamplerFilter::Linear,
		.addressModeU = AptnSamplerAddressMode::Repeat,
		.addressModeV = AptnSamplerAddressMode::Repeat,
		.mipMode = AptnSamplerMipmapMode::Linear,
		.maxAnisotropy = 16.f,
		.maxLod = (float)textures[0].mipLevels
	};
	Apparition::WriteSamplerDescriptor(samplerHeap, samplerDescriptor);

	samplerDescriptor.filter = AptnSamplerFilter::Nearest;
	samplerDescriptor.mipMode = AptnSamplerMipmapMode::Nearest;
	Apparition::WriteSamplerDescriptor(samplerHeap, samplerDescriptor);

	Apparition::CommitSamplerDescriptors(samplerHeap);

	// Resource Heap
	AptnResourceHeapCreationParams resourceHeapParams
	{
		.bufferCount = 2,
		.imageCount = 2
	};
	resourceHeap = Apparition::CreateResourceHeap(device, resourceHeapParams);

	const StaticArray<Vector4, 2> positions = { Vector4{-1.5f, 0.f, 0.f, 0.f}, Vector4{1.5, 0.f, 0.f, 0.f} };
	const StaticArray<Vector4, 2> colors = { Vector4{.5, 1.f, .5f, 0.f}, Vector4{.5f, .5f, 1.f, 0.f} };

	for (u32 i = 0; i < modelDataBuffers.Size(); ++i)
	{
		AptnBuffer& modelBuffer = modelDataBuffers[i];
		AptnBufferCreationParams params
		{
			.usage = AptnBufferUsageFlags::StorageBuffer | AptnBufferUsageFlags::ShaderDeviceAddress,
			.size = sizeof(ModelData),
			.supportsMappedMemory = true
		};
		modelBuffer = Apparition::CreateBuffer(device, params);

		ModelData modelData{ .pos = positions[i], .color = colors[i] };
		void* data = Apparition::MapBuffer(modelBuffer);
		Memcpy(data, &modelData, sizeof(ModelData));
		Apparition::UnmapBuffer(modelBuffer);

		AptnBufferAddressDescriptor addrDescriptor
		{
			.address = Apparition::GetBufferDeviceAddress(modelBuffer),
			.size = sizeof(ModelData),
			.type = AptnDescriptor::StorageBuffer
		};
		Apparition::WriteBufferAddressDescriptor(resourceHeap, addrDescriptor);
	}

	for (vks::Texture2D texture : textures)
	{
		AptnImageDescriptor imageDescriptor
		{
			.access = AptnImageAccess::ColorRead,
			.aspect = AptnImageAspectFlags::Color,
			.baseMipLevel = 0,
			.mipCount = texture.mipLevels,
			.format = texture.format,
			.image = texture.image,
			.type = AptnDescriptor::SampledImage
		};
		Apparition::WriteImageResourceDescriptor(resourceHeap, imageDescriptor);
	}

	Apparition::CommitResourceDescriptors(resourceHeap);

	uniformData.imageHeapIndexOffset = 1; //1;

	// Pipeline
	AptnVertexInputPipelineState vertexInput;
	{
		AptnVertexInputPipelineStateCreationParams params
		{
			.pipelineDesc = {.useDescriptorHeaps = true },
			.primitiveTopology = AptnPrimitiveTopology::TriangleList,
			.attributes = vkglTF::Vertex::inputAttributeDescriptions(0, {vkglTF::VertexComponent::Position, vkglTF::VertexComponent::Normal, vkglTF::VertexComponent::UV, vkglTF::VertexComponent::Color}),
			.bindings = { vkglTF::Vertex::inputBindingDescription(0) }
		};
		vertexInput = Apparition::CreateVertexInputPipelineState(device, params);
	}

	AptnPrerasterShadersPipelineState prerasterShaders;
	{
		const MemoryBuffer vertShaderCode = LoadShader("DescriptorHeaps/cube.vert.spv");
		DynamicArray<u32> vertShader(vertShaderCode.Size() / sizeof(u32));
		Memcpy(vertShader.GetData(), vertShaderCode.GetData(), vertShaderCode.Size());
		AptnPreRasterShadersPipelineStateCreationParams params
		{
			.pipelineDesc = { .useDescriptorHeaps = true },
			.fillMode = AptnFillMode::Full,
			.cullingMode = AptnCullMode::None,
			.frontFace = AptnFrontFace::CounterClockwise,
			.lineWidth = 1.f,
			.vertexShader =
			{
				.code = vertShader,
				.entryName = "main"
			}
		};

		prerasterShaders = Apparition::CreatePrerasterShadersPipelineState(device, params);
	}

	AptnFragmentShaderPipelineState fragmentShader;
	{
		MemoryBuffer fragShaderCode = LoadShader("DescriptorHeaps/cube.frag.spv");
		DynamicArray<u32> fragShader(fragShaderCode.Size() / sizeof(u32));
		Memcpy(fragShader.GetData(), fragShaderCode.GetData(), fragShaderCode.Size());
		const AptnFragmentShaderPipelineStateCreationParams params
		{
			.pipelineDesc = { .useDescriptorHeaps = true },
			.fragmentShader =
			{
				.code = fragShader,
				.entryName = "main"
			},
			.depthTestEnabled = true,
			.depthWriteEnabled = true,
			.depthCompareOp = AptnCompareOperation::LessThanOrEqual
		};

		fragmentShader = Apparition::CreateFragmentShaderPipelineState(device, params);
	}

	AptnFragmentOutputPipelineState fragmentOutput;
	{
		AptnFragmentOutputPipelineStateCreationParams params
		{
			.pipelineDesc{ .useDescriptorHeaps = true },
			.attachments
			{
				AptnColorBlendAttachment
				{
					.srcColorFactor = AptnBlendFactor::Zero,
					.dstColorFactor = AptnBlendFactor::Zero,
					.colorBlendOperation = AptnBlendOperation::None,
					.srcAlphaFactor = AptnBlendFactor::Zero,
					.dstAlphaFactor = AptnBlendFactor::Zero,
					.alphaBlendOperation = AptnBlendOperation::None,
					.colorMask = AptnColorComponentFlags::RGBA
				}
			},
			.colorAttachmentFormats
			{
				Apparition::GetBackbufferFormat(device),
			},
			.depthAttachmentFormat = AptnImageFormat::DS_32f_8u,
			.stencilAttachmentFormat = AptnImageFormat::DS_32f_8u
		};

		fragmentOutput = Apparition::CreateFragmentOutputPipelineState(device, params);
	}

	{
		AptnPipelineCreationParams params
		{
			.pipelineDesc = { .useDescriptorHeaps = true },
			.vertexInput = vertexInput,
			.prerasterShaders = prerasterShaders,
			.fragmentShader = fragmentShader,
			.fragmentOutput = fragmentOutput
		};

		pipeline = Apparition::CreatePipeline(device, params);
	}

	// Depth
	{
		AptnImageCreationParams imageParams
		{
			.format = AptnImageFormat::DS_32f_8u,
			.mipLevels = 1,
			.width = window->width,
			.height = window->height,
			.usageFlags = AptnImageUsageFlags::DepthStencilAttachment
		};
		depthStencilImage = Apparition::CreateImage(device, imageParams);

		AptnImageViewCreationParams viewParams
		{
			.aspect = AptnImageAspectFlags::DepthStencil,
			.format = AptnImageFormat::DS_32f_8u,
			.baseMipLevel = 0,
			.mipCount = 1
		};
		depthStencilView = Apparition::CreateImageView(depthStencilImage, viewParams);
	}

	// Draw Command Buffers
	{
		AptnCommandPoolCreationParams createParams{
			.queueIndex = Apparition::GetQueueIndex(graphicsQueue),
			.canResetCommandBuffers = true
		};
		commandPool = Apparition::CreateCommandPool(device, createParams);
	}

	{
		AptnCommandBufferAllocParams allocParams{
			.isSecondary = false
		};
		drawCommandBuffers[0] = Apparition::AllocateCommandBuffer(commandPool, allocParams);
		drawCommandBuffers[1] = Apparition::AllocateCommandBuffer(commandPool, allocParams);
	}
}

static u32 currentBuffer = 0;

void TickDescriptorHeapsExample(/*Apparition::Device device*/)
{
	AptnBackbufferStatus preparationStatus = Apparition::AcquireBackbufferImage(device);
	Assert(preparationStatus != AptnBackbufferStatus::Unavailable);
	AptnImageView backbufferView = Apparition::GetBackBufferImageView(device);

	const Matrix4 perspective = Math::ConstructPerspectiveMatrix(60.f, (float)window->width / (float)window->height, 0.1f, 512.f);
	const f32 radRotX = Math::DegreesToRadians(cameraRotationX);
	const f32 radRotY = Math::DegreesToRadians(cameraRotationY);
	constexpr bool orbit = true;
	const Matrix4 view = Math::ConstructViewMatrix(Vector4(0.f, 0.f, -5.f), Quat(ROT_XYZ, radRotX, radRotY, 0.f), orbit);

	uniformData.mvp = view * perspective;
	uniformData.samplerIndex = selectedSampler;

	void* data = Apparition::MapBuffer(uniformBuffers[currentBuffer]);
	memcpy(data, &uniformData, sizeof(UniformData));
	Apparition::UnmapBuffer(uniformBuffers[currentBuffer]);

	AptnCommandBuffer commandBuffer = drawCommandBuffers[currentBuffer];
	Apparition::BeginCommandBuffer(commandBuffer);

	{
		AptnImageMemoryBarrierDesc barrierDesc = {
			.image = Apparition::GetAcquiredBackbufferImage(device),
			.access = AptnImageAccess::ColorWrite,
			.aspect = AptnImageAspectFlags::Color,
			.mipLevelCount = 1
		};

		Apparition::ImageMemoryBarrier(commandBuffer, barrierDesc);

		barrierDesc = {
			.image = depthStencilImage,
			.access = AptnImageAccess::DepthStencilWrite,
			.aspect = AptnImageAspectFlags::DepthStencil,
			.mipLevelCount = 1
		};

		Apparition::ImageMemoryBarrier(commandBuffer, barrierDesc);
	}

	AptnRenderAttachment colorAttachment
	{
		.imageView = backbufferView,
		.loadStoreOps = AptnAttachmentOperations::Clear_Store,
		.clearValue = {{0.025f, 0.025f, 0.025f, 1.0f}}
	};

	AptnRenderAttachment depthAttachment
	{
		.imageView = depthStencilView,
		.loadStoreOps = AptnAttachmentOperations::Clear_Store,
		.clearValue = {.depthStencil{1.f, 0}}
	};

	const u32 backbufferWidth = Apparition::GetBackbufferWidth(device);
	const u32 backbufferHeight = Apparition::GetBackbufferHeight(device);


	AptnRenderSetupParams renderSetup = {};
	renderSetup.colorAttachments.Add(colorAttachment);
	renderSetup.depthAttachment = depthAttachment;
	renderSetup.renderWidth = backbufferWidth;
	renderSetup.renderHeight = backbufferHeight;
	Apparition::BeginRendering(commandBuffer, renderSetup);

	Apparition::BindGraphicsPipeline(commandBuffer, pipeline);

	AptnViewportDesc viewportDesc = {
	.x = 0.f,
	.y = 0.f,
	.width = static_cast<float>(backbufferWidth),
	.height = static_cast<float>(backbufferHeight)
	};
	AptnScissorDesc scissorDesc = {
		.offsetX = 0,
		.offsetY = 0,
		.extentX = backbufferWidth,
		.extentY = backbufferHeight
	};
	Apparition::SetViewportAndScissor(commandBuffer, viewportDesc, scissorDesc);

	Apparition::BindResourceHeap(commandBuffer, resourceHeap);
	Apparition::BindSamplerHeap(commandBuffer, samplerHeap);

	PushConstantBlock reference{};
	reference.matrixReference = Apparition::GetBufferDeviceAddress(uniformBuffers[currentBuffer]);
	AptnPushDataDesc pushData
	{
		.dataAddress = &reference,
		.dataSize = sizeof(PushConstantBlock)
	};
	Apparition::PushData(commandBuffer, pushData);

	model.bindBuffers(commandBuffer);
	const auto& primitive = model.nodeFromName("cube")->mesh[0].primitives[0];
	for (uint32_t i = 0; i < 2; ++i)
	{
		Apparition::DrawIndexed(commandBuffer, primitive->indexCount, primitive->firstIndex, 1, i);
	}

	Apparition::EndRendering(commandBuffer);

	{
		AptnImageMemoryBarrierDesc barrierDesc = {
			.image = Apparition::GetAcquiredBackbufferImage(device),
			.access = AptnImageAccess::Present,
			.aspect = AptnImageAspectFlags::Color,
			.mipLevelCount = 1
		};

		Apparition::ImageMemoryBarrier(commandBuffer, barrierDesc);
	}

	Apparition::EndCommandBuffer(commandBuffer);

	Apparition::SubmitBackbufferCommandBuffer(commandBuffer, graphicsQueue);
	Apparition::PresentBackbuffer(graphicsQueue);

	Apparition::WaitForIdle(graphicsQueue);

	currentBuffer = (currentBuffer + 1) % maxConcurrentFrames;
}

void DestroyDescriptorHeapsExample(/*Apparition::Device device*/)
{
	Apparition::WaitForIdle(graphicsQueue);

	Apparition::DestroyResourceHeap(resourceHeap);
	Apparition::DestroySamplerHeap(samplerHeap);

	for (auto& texture : textures)
	{
		texture.destroy();
	}

	model.releaseResources();

	Apparition::DestroyPipeline(pipeline);

	Apparition::FreeCommandBuffer(drawCommandBuffers[0]);
	Apparition::FreeCommandBuffer(drawCommandBuffers[1]);
	Apparition::DestroyCommandPool(commandPool);

	Apparition::DestroyImageView(depthStencilView);
	Apparition::DestroyImage(depthStencilImage);
	
	Apparition::DestroyBuffer(modelDataBuffers[0]);
	Apparition::DestroyBuffer(modelDataBuffers[1]);
	Apparition::DestroyBuffer(uniformBuffers[0]);
	Apparition::DestroyBuffer(uniformBuffers[1]);
}
