
#include "Base.h"

#include "Math/Vector2.hpp"
#include "Math/Vector3.hpp"

namespace
{
AptnDevice device;
AptnQueue graphicsQueue;
AptnCommandPool commandPool;

AptnPipeline pipeline;

AptnBuffer vertexBuffer;
AptnBuffer indexBuffer;

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

AptnPipeline CreateBasicGraphicsPipeline(AptnDevice deviceHandle, AptnImageFormat::Type backbufferFormat)
{
	// Vertex Input
	AptnVertexInputPipelineState vertexInput;
	{
		AptnVertexInputPipelineStateCreationParams params
		{
			.primitiveTopology = AptnPrimitiveTopology::TriangleList,
			.attributes{
				AptnVertexAttributeDescription{
					.location = 0,
					.binding = 0,
					.format = AptnVertexInputFormat::F32_2,
					.offset = offsetof(Vertex, pos)
				},
				AptnVertexAttributeDescription{
					.location = 1,
					.binding = 0,
					.format = AptnVertexInputFormat::F32_3,
					.offset = offsetof(Vertex, color)
				}
			},
			.bindings{{.binding = 0, .stride = sizeof(Vertex), .inputRate = AptnVertexInputRate::Vertex}}
		};

		vertexInput = Apparition::CreateVertexInputPipelineState(deviceHandle, params);
	}

	// PreRaster shader state
	AptnPrerasterShadersPipelineState prerasterShaders;
	{
		const MemoryBuffer vertShaderCode = LoadShader("Base/base.vert.spv");
		DynamicArray<u32> vertShader(vertShaderCode.Size() / sizeof(u32));
		Memcpy(vertShader.GetData(), vertShaderCode.GetData(), vertShaderCode.Size());
		AptnPreRasterShadersPipelineStateCreationParams params
		{
			//.pipelineDesc = No pipeline desc yet
			.fillMode = AptnFillMode::Full,
			.cullingMode = AptnCullMode::Back,
			.frontFace = AptnFrontFace::CounterClockwise,
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
	AptnFragmentShaderPipelineState fragmentShader;
	{
		MemoryBuffer fragShaderCode = LoadShader("Base/base.frag.spv");
		DynamicArray<u32> fragShader(fragShaderCode.Size() / sizeof(u32));
		Memcpy(fragShader.GetData(), fragShaderCode.GetData(), fragShaderCode.Size());
		const AptnFragmentShaderPipelineStateCreationParams params
		{
			//.pipelineDesc = No pipeline desc yet
			.fragmentShader =
			{
				.code = fragShader,
				.entryName = "main"
			},
			.depthTestEnabled = false,
			.depthWriteEnabled = false,
			.depthCompareOp = AptnCompareOperation::LessThanOrEqual
		};

		fragmentShader = Apparition::CreateFragmentShaderPipelineState(deviceHandle, params);
	}

	// Fragment Output
	AptnFragmentOutputPipelineState fragmentOutput;
	{
		AptnFragmentOutputPipelineStateCreationParams params
		{
			.attachments
			{
				AptnColorBlendAttachment
				{
					.srcColorFactor = AptnBlendFactor::One,
					.dstColorFactor = AptnBlendFactor::Zero,
					.colorBlendOperation = AptnBlendOperation::None,
					.srcAlphaFactor = AptnBlendFactor::One,
					.dstAlphaFactor = AptnBlendFactor::Zero,
					.alphaBlendOperation = AptnBlendOperation::None,
					.colorMask = AptnColorComponentFlagBits::RGBA
				}
			},
			.colorAttachmentFormats
			{
				backbufferFormat
			}
		};

		fragmentOutput = Apparition::CreateFragmentOutputPipelineState(deviceHandle, params);
	}

	AptnPipelineCreationParams params
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

void InitializeBaseExample(AptnDevice inDevice)
{
	device = inDevice;

	graphicsQueue = Apparition::AllocateGraphicsQueue(device);

	AptnBackbufferSetupParams backbufferSetupParams{
		.wndHandle = window->windowHandle,
		.wndWidth = window->width,
		.wndHeight = window->height
	};
	Apparition::SetupBackbuffer(device, backbufferSetupParams);
	AptnImageFormat::Type backbufferFormat = Apparition::GetBackbufferFormat(device);

	pipeline = CreateBasicGraphicsPipeline(device, backbufferFormat);

	// Command Buffer Setup
	{
		AptnCommandPoolCreationParams createParams{
			.queueIndex = Apparition::GetQueueIndex(graphicsQueue),
			.canResetCommandBuffers = true
		};
		commandPool = Apparition::CreateCommandPool(device, createParams);
	}

	// Vertex Buffer Setup
	{
		AptnBufferCreationParams params{
			.usage = AptnBufferUsageFlagBits::VertexBuffer | AptnBufferUsageFlagBits::TransferDst,
			.size = sizeof(vertices[0]) * vertices.Size()
		};
		vertexBuffer = Apparition::CreateBuffer(device, params);
	}

	// Index Buffer Setup
	{
		AptnBufferCreationParams params{
			.usage = AptnBufferUsageFlagBits::IndexBuffer | AptnBufferUsageFlagBits::TransferDst,
			.size = sizeof(vertices[0]) * vertices.Size()
		};
		indexBuffer = Apparition::CreateBuffer(device, params);
	}

	{
		// Copy verts && indices
		AptnBuffer vertStagingBuffer;
		{
			AptnBufferCreationParams params{
				.usage = AptnBufferUsageFlagBits::TransferSrc,
				.size = sizeof(vertices[0]) * vertices.Size(),
				.supportsMappedMemory = true
			};
			vertStagingBuffer = Apparition::CreateBuffer(device, params);

			void* data = Apparition::MapBuffer(vertStagingBuffer);
			Memcpy(data, vertices.internalData, vertices.Size() * sizeof(Vertex));
			Apparition::UnmapBuffer(vertStagingBuffer);
			data = nullptr;
		}

		AptnBuffer idxStagingBuffer;
		{
			AptnBufferCreationParams params{
				.usage = AptnBufferUsageFlagBits::TransferSrc,
				.size = sizeof(indices[0]) * indices.Size(),
				.supportsMappedMemory = true
			};
			idxStagingBuffer = Apparition::CreateBuffer(device, params);

			void* data = Apparition::MapBuffer(idxStagingBuffer);
			Memcpy(data, indices.internalData, indices.Size() * sizeof(indices[0]));
			Apparition::UnmapBuffer(idxStagingBuffer);
			data = nullptr;
		}

		AptnCommandBuffer copyBuffer;
		{
			AptnCommandBufferAllocParams params{
				.isSecondary = false
			};
			copyBuffer = Apparition::AllocateCommandBuffer(commandPool, params);
		}

		Apparition::BeginCommandBuffer(copyBuffer, /* oneTime= */true);

		{
			AptnBufferCopyDesc copyDesc{
				.size = vertices.Size() * sizeof(Vertex),
				.srcBuffer = vertStagingBuffer,
				.dstBuffer = vertexBuffer,
				.srcOffset = 0,
				.dstOffset = 0
			};
			Apparition::CopyBuffer(copyBuffer, copyDesc);
		}

		{
			AptnBufferCopyDesc copyDesc{
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

void TickBaseExample(/*AptnDevice device*/)
{
	AptnBackbufferStatus preparationStatus = Apparition::AcquireBackbufferImage(device);
	Assert(preparationStatus != AptnBackbufferStatus::Unavailable);
	AptnImageView backbufferView = Apparition::GetBackBufferImageView(device);

	AptnCommandBuffer commandBuffer;
	{
		AptnCommandBufferAllocParams allocParams{
			.isSecondary = false
		};
		commandBuffer = Apparition::AllocateCommandBuffer(commandPool, allocParams);
	}

	Apparition::BeginCommandBuffer(commandBuffer);

	{
		AptnImageMemoryBarrierDesc barrierDesc = {
			.image = Apparition::GetAcquiredBackbufferImage(device),
			.access = AptnImageAccess::ColorWrite,
			.aspect = AptnImageAspect::Color,
			.mipLevelCount = 1
		};

		Apparition::ImageMemoryBarrier(commandBuffer, barrierDesc);
	}

	AptnRenderAttachment colorAttachment
	{
		.imageView = backbufferView,
		.loadStoreOps = AptnAttachmentOperations::Clear_Store,
		.clearValue = {{.5f, .5f, .5f, 1.f}}
	};

	const u32 backbufferWidth = Apparition::GetBackbufferWidth(device);
	const u32 backbufferHeight = Apparition::GetBackbufferHeight(device);

	AptnRenderSetupParams renderSetup = {};
	renderSetup.colorAttachments.Add(colorAttachment);
	renderSetup.renderWidth = backbufferWidth;
	renderSetup.renderHeight = backbufferHeight;
	Apparition::BeginRendering(commandBuffer, renderSetup);

	Apparition::BindGraphicsPipeline(commandBuffer, pipeline);

	{
		AptnBindVertexBufferDesc desc = {
			.vertexBuffer = vertexBuffer
		};
		Apparition::BindVertexBuffers(commandBuffer, desc);
	}

	{
		AptnBindIndexBufferDesc desc = {
			.indexBuffer = indexBuffer
		};
		Apparition::BindIndexBuffer(commandBuffer, desc);
	}

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

	// Draw
	Apparition::DrawIndexed(commandBuffer, (u32)indices.Size());

	// End rendering so we can transition backbuffer
	Apparition::EndRendering(commandBuffer);

	// Prep image for present
	{
		AptnImageMemoryBarrierDesc barrierDesc = {
			.image = Apparition::GetAcquiredBackbufferImage(device),
			.access = AptnImageAccess::Present,
			.aspect = AptnImageAspect::Color,
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

void DestroyBaseExample(/*AptnDevice device*/)
{
	Apparition::WaitForIdle(graphicsQueue);

	Apparition::DestroyPipeline(pipeline);

	Apparition::DestroyCommandPool(commandPool);

	Apparition::DestroyBuffer(indexBuffer);
	Apparition::DestroyBuffer(vertexBuffer);
}