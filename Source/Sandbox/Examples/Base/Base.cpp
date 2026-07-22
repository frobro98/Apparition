
#include "Base.h"

#include "Math/Vector2.hpp"
#include "Math/Vector3.hpp"

namespace
{
Apparition::Device device;
Apparition::Queue graphicsQueue;
Apparition::CommandPool commandPool;

Apparition::Pipeline pipeline;

Apparition::Buffer vertexBuffer;
Apparition::Buffer indexBuffer;

struct Vertex
{
	Vector2 pos;
	Vector3 color;
};

static const StaticArray<Vertex, 4> vertices = {
	Vertex{Vector2{-0.5f, -0.5f}, Vector3{1.0f, 0.0f, 0.0f}},
	Vertex{Vector2{0.5f, -0.5f}, Vector3{0.0f, 1.0f, 0.0f}},
	Vertex{Vector2{0.5f, 0.5f}, Vector3{0.0f, 0.0f, 1.0f}},
	Vertex{Vector2{-0.5f, 0.5f}, Vector3{1.0f, 1.0f, 1.0f}}
};

static const StaticArray<u16, 6> indices = {
	 2, 1, 0, 0, 3, 2
};

Apparition::Pipeline CreateBasicGraphicsPipeline(Apparition::Device deviceHandle, Apparition::ImageFormat::Type backbufferFormat)
{
	// Vertex Input
	Apparition::VertexInputPipelineState vertexInput;
	{
		Apparition::VertexInputPipelineStateCreationParams params
		{
			.primitiveTopology = Apparition::PrimitiveTopology::TriangleList,
			.attributes{
				Apparition::VertexAttributeDescription{
					.location = 0,
					.binding = 0,
					.format = Apparition::VertexInputFormat::F32_2,
					.offset = offsetof(Vertex, pos)
				},
				Apparition::VertexAttributeDescription{
					.location = 1,
					.binding = 0,
					.format = Apparition::VertexInputFormat::F32_3,
					.offset = offsetof(Vertex, color)
				}
			},
			.bindings{{.binding = 0, .stride = sizeof(Vertex), .inputRate = Apparition::VertexInputRate::Vertex}}
		};

		vertexInput = Apparition::CreateVertexInputPipelineState(deviceHandle, params);
	}

	// PreRaster shader state
	Apparition::PrerasterShadersPipelineState prerasterShaders;
	{
		const MemoryBuffer vertShaderCode = LoadShader("Base/base.vert.spv");
		DynamicArray<u32> vertShader(vertShaderCode.Size() / sizeof(u32));
		Memcpy(vertShader.GetData(), vertShaderCode.GetData(), vertShaderCode.Size());
		Apparition::PreRasterShadersPipelineStateCreationParams params
		{
			//.pipelineDesc = No pipeline desc yet
			.fillMode = Apparition::FillMode::Full,
			.cullingMode = Apparition::CullMode::Back,
			.frontFace = Apparition::FrontFace::CounterClockwise,
			.lineWidth = 1.f,
			.vertexShader =
			{
				.code = vertShader,
				.entryName = "main"
			}
		};

		prerasterShaders = Apparition::CreatePrerasterShadersPipelineState(deviceHandle, params);
	}

	// Fragment Shader
	Apparition::FragmentShaderPipelineState fragmentShader;
	{
		MemoryBuffer fragShaderCode = LoadShader("Base/base.frag.spv");
		DynamicArray<u32> fragShader(fragShaderCode.Size() / sizeof(u32));
		Memcpy(fragShader.GetData(), fragShaderCode.GetData(), fragShaderCode.Size());
		const Apparition::FragmentShaderPipelineStateCreationParams params
		{
			//.pipelineDesc = No pipeline desc yet
			.fragmentShader =
			{
				.code = fragShader,
				.entryName = "main"
			},
			.depthTestEnabled = false,
			.depthWriteEnabled = false,
			.depthCompareOp = Apparition::CompareOperation::LessThanOrEqual
		};

		fragmentShader = Apparition::CreateFragmentShaderPipelineState(deviceHandle, params);
	}

	// Fragment Output
	Apparition::FragmentOutputPipelineState fragmentOutput;
	{
		Apparition::FragmentOutputPipelineStateCreationParams params
		{
			.attachments
			{
				Apparition::ColorBlendAttachment
				{
					.srcColorFactor = Apparition::BlendFactor::One,
					.dstColorFactor = Apparition::BlendFactor::Zero,
					.colorBlendOperation = Apparition::BlendOperation::None,
					.srcAlphaFactor = Apparition::BlendFactor::One,
					.dstAlphaFactor = Apparition::BlendFactor::Zero,
					.alphaBlendOperation = Apparition::BlendOperation::None,
					.colorMask = Apparition::ColorComponentFlagBits::RGBA
				}
			},
			.colorAttachmentFormats
			{
				backbufferFormat
			}
		};

		fragmentOutput = Apparition::CreateFragmentOutputPipelineState(deviceHandle, params);
	}

	Apparition::PipelineCreationParams params
	{
		//.pipelineDesc = No pipeline desc yet

		.vertexInput = vertexInput,
		.prerasterShaders = prerasterShaders,
		.fragmentShader = fragmentShader,
		.fragmentOutput = fragmentOutput
	};
	return Apparition::CreatePipeline(deviceHandle, params);
}
}

void InitializeBaseExample(Apparition::Device inDevice)
{
	device = inDevice;

	graphicsQueue = Apparition::AllocateGraphicsQueue(device);

	Apparition::BackbufferSetupParams backbufferSetupParams{
		.wndHandle = window->windowHandle,
		.wndWidth = window->width,
		.wndHeight = window->height
	};
	Apparition::SetupBackbuffer(device, backbufferSetupParams);
	Apparition::ImageFormat::Type backbufferFormat = Apparition::GetBackbufferFormat(device);

	pipeline = CreateBasicGraphicsPipeline(device, backbufferFormat);

	// Command Buffer Setup
	{
		Apparition::CommandPoolCreationParams createParams{
			.queueIndex = Apparition::GetGraphicsQueueIndex(device),
			.canResetCommandBuffers = true
		};
		commandPool = Apparition::CreateCommandPool(device, createParams);
	}

	// Vertex Buffer Setup
	{
		Apparition::BufferCreationParams params{
			.usage = Apparition::BufferUsageFlagBits::VertexBuffer | Apparition::BufferUsageFlagBits::TransferDst,
			.size = sizeof(vertices[0]) * vertices.Size()
		};
		vertexBuffer = Apparition::CreateBuffer(device, params);
	}

	// Index Buffer Setup
	{
		Apparition::BufferCreationParams params{
			.usage = Apparition::BufferUsageFlagBits::IndexBuffer | Apparition::BufferUsageFlagBits::TransferDst,
			.size = sizeof(vertices[0]) * vertices.Size()
		};
		indexBuffer = Apparition::CreateBuffer(device, params);
	}

	{
		// Copy verts && indices
		Apparition::Buffer vertStagingBuffer;
		{
			Apparition::BufferCreationParams params{
				.usage = Apparition::BufferUsageFlagBits::TransferSrc,
				.size = sizeof(vertices[0]) * vertices.Size(),
				.supportsMappedMemory = true
			};
			vertStagingBuffer = Apparition::CreateBuffer(device, params);

			void* data = Apparition::MapBuffer(vertStagingBuffer);
			Memcpy(data, vertices.internalData, vertices.Size() * sizeof(Vertex));
			Apparition::UnmapBuffer(vertStagingBuffer);
			data = nullptr;
		}

		Apparition::Buffer idxStagingBuffer;
		{
			Apparition::BufferCreationParams params{
				.usage = Apparition::BufferUsageFlagBits::TransferSrc,
				.size = sizeof(indices[0]) * indices.Size(),
				.supportsMappedMemory = true
			};
			idxStagingBuffer = Apparition::CreateBuffer(device, params);

			void* data = Apparition::MapBuffer(idxStagingBuffer);
			Memcpy(data, indices.internalData, indices.Size() * sizeof(indices[0]));
			Apparition::UnmapBuffer(idxStagingBuffer);
			data = nullptr;
		}

		Apparition::CommandBuffer copyBuffer;
		{
			Apparition::CommandBufferAllocParams params{
				.isSecondary = false
			};
			copyBuffer = Apparition::AllocateCommandBuffer(commandPool, params);
		}

		Apparition::BeginCommandBuffer(copyBuffer, /* oneTime= */true);

		{
			Apparition::BufferCopyDesc copyDesc{
				.size = vertices.Size() * sizeof(Vertex),
				.srcBuffer = vertStagingBuffer,
				.dstBuffer = vertexBuffer,
				.srcOffset = 0,
				.dstOffset = 0
			};
			Apparition::CopyBuffer(copyBuffer, copyDesc);
		}

		{
			Apparition::BufferCopyDesc copyDesc{
				.size = indices.Size() * sizeof(u16),
				.srcBuffer = idxStagingBuffer,
				.dstBuffer = indexBuffer,
				.srcOffset = 0,
				.dstOffset = 0
			};
			Apparition::CopyBuffer(copyBuffer, copyDesc);
		}

		Apparition::EndCommandBuffer(copyBuffer);

		Apparition::SubmitCommandBuffer(graphicsQueue, copyBuffer);
		Apparition::WaitForIdle(graphicsQueue);

		Apparition::FreeCommandBuffer(copyBuffer);
		Apparition::DestroyBuffer(vertStagingBuffer);
		Apparition::DestroyBuffer(idxStagingBuffer);
	}
}

void TickBaseExample(/*Apparition::Device device*/)
{
	Apparition::BackbufferStatus preparationStatus = Apparition::AcquireBackbufferImage(device);
	Assert(preparationStatus != Apparition::BackbufferStatus::Unavailable);
	Apparition::ImageView backbufferView = Apparition::GetBackBufferImageView(device);

	Apparition::CommandBuffer commandBuffer;
	{
		Apparition::CommandBufferAllocParams allocParams{
			.isSecondary = false
		};
		commandBuffer = Apparition::AllocateCommandBuffer(commandPool, allocParams);
	}

	Apparition::BeginCommandBuffer(commandBuffer);

	{
		Apparition::ImageMemoryBarrierDesc barrierDesc = {
			.image = Apparition::GetAcquiredBackbufferImage(device),
			.access = Apparition::ImageAccess::ColorWrite,
			.aspect = Apparition::ImageAspect::Color,
			.mipLevelCount = 1
		};

		Apparition::ImageMemoryBarrier(commandBuffer, barrierDesc);
	}

	Apparition::RenderAttachment colorAttachment
	{
		.imageView = backbufferView,
		.loadStoreOps = Apparition::AttachmentOperations::Clear_Store,
		.clearValue = {{.5f, .5f, .5f, 1.f}}
	};

	const u32 backbufferWidth = Apparition::GetBackbufferWidth(device);
	const u32 backbufferHeight = Apparition::GetBackbufferHeight(device);

	Apparition::RenderSetupParams renderSetup = {};
	renderSetup.colorAttachments.Add(colorAttachment);
	renderSetup.renderWidth = backbufferWidth;
	renderSetup.renderHeight = backbufferHeight;
	Apparition::BeginRendering(commandBuffer, renderSetup);

	Apparition::BindGraphicsPipeline(commandBuffer, pipeline);

	{
		Apparition::BindVertexBufferDesc desc = {
			.vertexBuffer = vertexBuffer
		};
		Apparition::BindVertexBuffers(commandBuffer, desc);
	}

	{
		Apparition::BindIndexBufferDesc desc = {
			.indexBuffer = indexBuffer
		};
		Apparition::BindIndexBuffer(commandBuffer, desc);
	}

	Apparition::ViewportDesc viewportDesc = {
		.x = 0.f,
		.y = 0.f,
		.width = static_cast<float>(backbufferWidth),
		.height = static_cast<float>(backbufferHeight)
	};
	Apparition::ScissorDesc scissorDesc = {
		.offsetX = 0,
		.offsetY = 0,
		.extentX = backbufferWidth,
		.extentY = backbufferHeight
	};
	Apparition::SetViewportAndScissor(commandBuffer, viewportDesc, scissorDesc);

	// Draw
	Apparition::DrawIndexed(commandBuffer, (u32)indices.Size());

	// End rendering so we can transition backbuffer
	Apparition::EndRendering(commandBuffer);

	// Prep image for present
	{
		Apparition::ImageMemoryBarrierDesc barrierDesc = {
			.image = Apparition::GetAcquiredBackbufferImage(device),
			.access = Apparition::ImageAccess::Present,
			.aspect = Apparition::ImageAspect::Color,
			.mipLevelCount = 1
		};

		Apparition::ImageMemoryBarrier(commandBuffer, barrierDesc);
	}

	Apparition::EndCommandBuffer(commandBuffer);

	// End Render Frame
	// Submit Command Buffer and Present
	Apparition::SubmitBackbufferCommandBuffer(commandBuffer, graphicsQueue);
	Apparition::PresentBackbuffer(graphicsQueue);

	Apparition::WaitForIdle(graphicsQueue);

	Apparition::FreeCommandBuffer(commandBuffer);
}

void DestroyBaseExample(/*Apparition::Device device*/)
{
	Apparition::WaitForIdle(graphicsQueue);

	Apparition::DestroyPipeline(pipeline);

	Apparition::DestroyCommandPool(commandPool);

	Apparition::DestroyBuffer(indexBuffer);
	Apparition::DestroyBuffer(vertexBuffer);
}