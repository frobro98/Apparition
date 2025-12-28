// Sandbox.cpp : Defines the entry point for the application.

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <limits>

#include "CoreFlags.hpp"

#define VK_PLATFORM_SURFACE_EXTENSION VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
WALL_WRN_PUSH
#define VMA_VULKAN_VERSION 1002000
#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"
WALL_WRN_POP

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/DynamicArray.hpp"
#include "Containers/MemoryBuffer.hpp"
#include "Containers/StaticArray.hpp"
#include "File/DirectoryLocations.hpp"
#include "File/FileSystem.hpp"
#include "Logging/LogCore.hpp"
#include "Logging/LogFunctions.hpp"
#include "Logging/Sinks/DebugOutputWindowSink.hpp"
#include "Math/Vector2.hpp"
#include "Math/Vector3.hpp"
#include "Path/Path.hpp"
#include "Utilities/Array.hpp"

// Apparition
#include "Apparition/ApparitionCore.h"
#include "Apparition/Device.h"
#include "Apparition/Backbuffer.h"
#include "Apparition/Buffer.h"
#include "Apparition/CommandBuffer.h"
#include "Apparition/CommandBufferCommands.h"
#include "Apparition/Queue.h"

// Sandbox
#include "Window.h"

DEFINE_LOG_CHANNEL(VkValidation);

#define CHECK_VK(expression) Assert(expression == VK_SUCCESS)

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

static VkBool32 VulkanDebugMessengerCallback(
	ValidationSeverity messageSeverity,
	u32 messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* /*pUserData*/
)
{
	if (pCallbackData->pMessageIdName)
	{
		bool shouldLog = Strncmp(pCallbackData->pMessageIdName, "UNASSIGNED", 10) != 0 &&
			Strncmp(pCallbackData->pMessageIdName, "Loader", 6) != 0;
		if (shouldLog)
		{
			UNUSED(messageType);
			const char* typeStr = "";
			if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
			{
				typeStr = "GEN";
			}
			else
			{
				if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
				{
					typeStr = "VALID";
				}
				if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
				{
					if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
					{
						typeStr = "VALID|PERF";
					}
					else
					{
						typeStr = "PERF";
					}
				}
			}

			if (messageSeverity == ValidationSeverity::Error)
			{
				MUSA_ERR(VkValidation, " {} : VUID({}): {}", typeStr, pCallbackData->pMessageIdName, pCallbackData->pMessage);
			}
			else if (messageSeverity == ValidationSeverity::Warning)
			{
				MUSA_WARN(VkValidation, " {} : VUID({}): {}", typeStr, pCallbackData->pMessageIdName, pCallbackData->pMessage);
			}
			else if (messageSeverity == ValidationSeverity::Info)
			{
				MUSA_INFO(VkValidation, " {} : VUID({}): {}", typeStr, pCallbackData->pMessageIdName, pCallbackData->pMessage);
			}
			else // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
			{
				MUSA_DEBUG(VkValidation, " {} : VUID({}): {}", typeStr, pCallbackData->pMessageIdName, pCallbackData->pMessage);
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

// Can be reused based on specific parts of the create info
struct Pipeline
{
	VkPipeline vkPipeline = VK_NULL_HANDLE;
	// Independent of pipeline since it just describes the layout
	VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
};

struct Vertex
{
	Vector2 pos;
	Vector3 color;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static StaticArray<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		StaticArray<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
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

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

void CreateBasicGraphicsPipeline(VkDevice device, VkFormat swapchainFormat, Pipeline& pipeline)
{
	MemoryBuffer vertShaderCode;
	MemoryBuffer fragShaderCode;
	{
		FileSystem::Handle vertHandle;
		if (FileSystem::OpenFile(vertHandle, "vert.spv", FileMode::Read))
		{
			u64 fileSize = FileSystem::FileSize(vertHandle);
			MemoryBuffer file{ fileSize };
			if (FileSystem::ReadFile(vertHandle, file.GetData(), (u32)fileSize))
			{
				file.CopyTo(vertShaderCode);
			}

			FileSystem::CloseFile(vertHandle);
		}

		FileSystem::Handle fragHandle;
		if (FileSystem::OpenFile(fragHandle, "frag.spv", FileMode::Read))
		{
			u64 fileSize = FileSystem::FileSize(fragHandle);
			MemoryBuffer file{ fileSize };
			if (FileSystem::ReadFile(fragHandle, file.GetData(), (u32)fileSize))
			{
				file.CopyTo(fragShaderCode);
			}

			FileSystem::CloseFile(fragHandle);
		}
	}

	VkShaderModule vertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;

	VkShaderModuleCreateInfo shaderInfo = {};
	shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderInfo.codeSize = vertShaderCode.Size();
	// Careful with this...
	shaderInfo.pCode = (const u32*)vertShaderCode.GetData();
	VkResult result = vkCreateShaderModule(device, &shaderInfo, nullptr, &vertexShaderModule);

	shaderInfo.codeSize = fragShaderCode.Size();
	// Careful with this...
	shaderInfo.pCode = (const u32*)fragShaderCode.GetData();
	result = vkCreateShaderModule(device, &shaderInfo, nullptr, &fragmentShaderModule);

	////////////////////////
	// Pipeline Creation
	////////////////////////

	// Shader Module
	VkPipelineShaderStageCreateInfo vertStageInfo = {};
	vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStageInfo.module = vertexShaderModule;
	vertStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragStageInfo = {};
	fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageInfo.module = fragmentShaderModule;
	fragStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertStageInfo, fragStageInfo };

	// Dynamic State
	DynamicArray<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.Size());
	dynamicState.pDynamicStates = dynamicStates.GetData();

	// Vertex Input
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.Size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.internalData;

	// Input Assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	// Viewport State
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	// Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	// Depth/Stencil

	// Color Blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	// Pipeline Layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
	result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipeline.vkPipelineLayout);

	// Dynamic Rendering Setup
	VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
		.viewMask = 0,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &swapchainFormat,
		.depthAttachmentFormat = VK_FORMAT_UNDEFINED,
		.stencilAttachmentFormat = VK_FORMAT_UNDEFINED
	};

	// Graphics Pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;

	pipelineInfo.layout = pipeline.vkPipelineLayout;
	// Using dynamic rendering, no need for render passes
	pipelineInfo.renderPass = VK_NULL_HANDLE; //renderPass.vkRenderPass;
	pipelineInfo.subpass = 0;

	// These will most likely never be used in reality
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	pipelineInfo.pNext = &pipelineRenderingCreateInfo;

	result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.vkPipeline);
	CHECK_VK(result);
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE /*hPrevInstance*/,
	LPSTR /*lpCmdLine*/,
	int /*nCmdShow*/)
{
	// TODO - This should be named whatever the game name is determined to be
	Path logFilePath = Path(EngineLogPath()) / "musa.log";

#if M_DEBUG
	GetLogger().InitLogging(LogLevel::Debug);
#else
	GetLogger().InitLogging(LogLevel::Info);
#endif

	GetLogger().AddLogSink(new DebugOutputWindowSink);

	static Window* window = nullptr;
	const u32 windowWidth = 1080;
	const u32 windowHeight = 720;
	window = CreateSandboxWindow(hInstance, 0, 0, windowWidth, windowHeight);


	Apparition::InitializeParams initParams{
		.applicationName = "Apparition Sandbox",
		.engineName = "Apparition",
		.applicationVersion = 0,
		.engineVersion = 0,
		.vulkanAPIVersion = APPARITION_MAKE_VERSION(1,3,290)
	};

	Apparition::ValidationDelegate debugCallback = &VulkanDebugMessengerCallback;
	Apparition::SetErrorLogCallback(MOVE(debugCallback), nullptr);
	Apparition::InitializeApparition(initParams);

	Apparition::Device deviceHandle;
	{
		Apparition::DeviceCreationParams createParams{
			.graphicsSupport = true,
			.computeSupport = true,
			.transferSupport = true,
		};
		deviceHandle = Apparition::CreateDevice(createParams);
	}

	Apparition::Queue graphicsQueue;
	{
		graphicsQueue = Apparition::AllocateGraphicsQueue(deviceHandle);
	}

	Apparition::BackbufferSetupParams backbufferSetupParams{
		.wndHandle = window->windowHandle,
		.wndWidth = windowWidth,
		.wndHeight = windowHeight
	};
	Apparition::SetupBackbuffer(deviceHandle, backbufferSetupParams);

	Pipeline pipeline = {};
	CreateBasicGraphicsPipeline(Apparition::GetVulkanDevice(deviceHandle), (VkFormat)Apparition::GetBackbufferVkFormat(deviceHandle), pipeline);

	// Command Buffer Setup
	Apparition::CommandPool cmdPoolHandle;
	{
		Apparition::CommandPoolCreationParams createParams{
			.queueIndex = Apparition::GetGraphicsQueueIndex(deviceHandle),
			.canResetCommandBuffers = true
			
		};
		cmdPoolHandle = Apparition::CreateCommandPool(deviceHandle, createParams);
	}

	Apparition::CommandBuffer cmdBufferHandle;
	{
		Apparition::CommandBufferAllocParams allocParams{
			.isSecondary = false
		};
		cmdBufferHandle = Apparition::AllocateCommandBuffer(cmdPoolHandle, allocParams);
	}

	// Vertex and Index Buffer Setup
	Apparition::Buffer vertexBufferHandle;
	{
		Apparition::BufferCreationParams params{
			.usage = Apparition::BufferUsageFlagBits::VertexBuffer | Apparition::BufferUsageFlagBits::TransferDst,
			.size = sizeof(vertices[0]) * vertices.Size()
		};
		vertexBufferHandle = Apparition::CreateBuffer(deviceHandle, params);
	}

	Apparition::Buffer indexBufferHandle;
	{
		Apparition::BufferCreationParams params{
			.usage = Apparition::BufferUsageFlagBits::IndexBuffer | Apparition::BufferUsageFlagBits::TransferDst,
			.size = sizeof(vertices[0]) * vertices.Size()
		};
		indexBufferHandle = Apparition::CreateBuffer(deviceHandle, params);
	}

	{
		// Copy verts
		Apparition::Buffer vertStagingBufferHandle;
		{
			Apparition::BufferCreationParams params{
				.usage = Apparition::BufferUsageFlagBits::TransferSrc,
				.size = sizeof(vertices[0]) * vertices.Size(),
				.supportsMappedMemory = true
			};
			vertStagingBufferHandle = Apparition::CreateBuffer(deviceHandle, params);
		}
		void* data = Apparition::MapBuffer(vertStagingBufferHandle);

		Memcpy(data, vertices.internalData, vertices.Size() * sizeof(Vertex));

		Apparition::UnmapBuffer(vertStagingBufferHandle);
		data = nullptr;

		Apparition::CommandBuffer copyVerts;
		{
			Apparition::CommandBufferAllocParams params{
				.isSecondary = false
			};
			copyVerts = Apparition::AllocateCommandBuffer(cmdPoolHandle, params);
		}

		Apparition::BeginCommandBuffer(copyVerts, /* oneTime= */true);

		{
			Apparition::BufferCopyDesc copyDesc{
				.size = vertices.Size() * sizeof(Vertex),
				.srcBuffer = vertStagingBufferHandle,
				.dstBuffer = vertexBufferHandle,
				.srcOffset = 0,
				.dstOffset = 0
			};
			Apparition::CopyBuffer(copyVerts, copyDesc);
		}

		Apparition::EndCommandBuffer(copyVerts);

		Apparition::SubmitCommandBuffer(graphicsQueue, copyVerts);
		Apparition::WaitForIdle(graphicsQueue);

		Apparition::FreeCommandBuffer(copyVerts);
		Apparition::DestroyBuffer(vertStagingBufferHandle);
	}

	{
		// Copy indices
		Apparition::Buffer idxStagingBufferHandle;
		{
			Apparition::BufferCreationParams params{
				.usage = Apparition::BufferUsageFlagBits::TransferSrc,
				.size = sizeof(indices[0]) * indices.Size(),
				.supportsMappedMemory = true
			};
			idxStagingBufferHandle = Apparition::CreateBuffer(deviceHandle, params);
		}

		void* data = Apparition::MapBuffer(idxStagingBufferHandle);

		Memcpy(data, indices.internalData, indices.Size() * sizeof(indices[0]));

		Apparition::UnmapBuffer(idxStagingBufferHandle);
		data = nullptr;


		Apparition::CommandBuffer copyIndices;
		{
			Apparition::CommandBufferAllocParams params{
				.isSecondary = false
			};
			copyIndices = Apparition::AllocateCommandBuffer(cmdPoolHandle, params);
		}

		Apparition::BeginCommandBuffer(copyIndices, /* oneTime= */true);

		{
			Apparition::BufferCopyDesc copyDesc{
				.size = indices.Size() * sizeof(u16),
				.srcBuffer = idxStagingBufferHandle,
				.dstBuffer = indexBufferHandle,
				.srcOffset = 0,
				.dstOffset = 0
			};
			Apparition::CopyBuffer(copyIndices, copyDesc);
		}

		Apparition::EndCommandBuffer(copyIndices);

		Apparition::SubmitCommandBuffer(graphicsQueue, copyIndices);
		Apparition::WaitForIdle(graphicsQueue);

		Apparition::FreeCommandBuffer(copyIndices);
		Apparition::DestroyBuffer(idxStagingBufferHandle);
	}

	VkCommandBuffer vkCmdBuffer = Apparition::GetVulkanHandle(cmdBufferHandle);

	while (true)
	{
		Apparition::BackbufferStatus preparationStatus = Apparition::StartRenderFrame(deviceHandle);
		Assert(preparationStatus != Apparition::BackbufferStatus::Unavailable);
		Apparition::ImageView backbufferView = Apparition::GetBackBufferImageView(deviceHandle);


		Apparition::ResetCommandBuffer(cmdBufferHandle);

		// Begin Command Buffer
		Apparition::BeginCommandBuffer(cmdBufferHandle);

		{
			Apparition::ImageMemoryBarrierDesc barrierDesc = {
				.image = Apparition::GetAcquiredBackbufferImage(deviceHandle),
				.access = Apparition::ImageAccess::ColorWrite,
				.aspect = Apparition::ImageViewAspect::Color
			};

			Apparition::ImageMemoryBarrier(cmdBufferHandle, barrierDesc);
		}

		Apparition::RenderAttachment colorAttachment = {
			.imageView = backbufferView,
			.loadStoreOps = Apparition::RenderAttachmentOperations::Clear_Store,
			.clearValue = {{.5f, .5f, .5f, 1.f}}
		};

		const u32 backbufferWidth = Apparition::GetBackbufferWidth(deviceHandle);
		const u32 backbufferHeight = Apparition::GetBackbufferHeight(deviceHandle);

		Apparition::RenderSetupParams renderSetup = {};
		renderSetup.colorAttachments.Add(colorAttachment);
		renderSetup.renderWidth = backbufferWidth;
		renderSetup.renderHeight = backbufferHeight;
		Apparition::BeginRendering(cmdBufferHandle, renderSetup);

		vkCmdBindPipeline(vkCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.vkPipeline);

		{
			Apparition::BindVertexBufferDesc desc = {
				.vertexBuffer = vertexBufferHandle
			};
			Apparition::BindVertexBuffers(cmdBufferHandle, desc);
		}

		{
			Apparition::BindIndexBufferDesc desc = {
				.indexBuffer = indexBufferHandle
			};
			Apparition::BindIndexBuffer(cmdBufferHandle, desc);
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
		Apparition::SetViewportAndScissor(cmdBufferHandle, viewportDesc, scissorDesc);

		// Draw
		Apparition::DrawIndexed(cmdBufferHandle, (u32)indices.Size());

		// End rendering so we can transition backbuffer
		Apparition::EndRendering(cmdBufferHandle);

		// Prep image for present
		{
			Apparition::ImageMemoryBarrierDesc barrierDesc = {
				.image = Apparition::GetAcquiredBackbufferImage(deviceHandle),
				.access = Apparition::ImageAccess::Present,
				.aspect = Apparition::ImageViewAspect::Color
			};

			Apparition::ImageMemoryBarrier(cmdBufferHandle, barrierDesc);
		}

		Apparition::EndCommandBuffer(cmdBufferHandle);

		// End Render Frame
		// Submit Command Buffer and Present
		Apparition::EndRenderFrame(cmdBufferHandle, graphicsQueue);
	}

	//vkDeviceWaitIdle(device.vkDevice);

	Apparition::TeardownBackbuffer(deviceHandle);
	Apparition::DestroyDevice(deviceHandle);

	// Things that are needed for support
	//   - Function-based api
	//   - Explicit device creation and management by user of api
	//   - Pipeline creation and caching
	//     - What does this mean?
	//       - Pipeline setup done via user via api
	//       - Caching is available in the background, but is opt in
	//         - Requires setup on user's end to opt in. Otherwise, there won't be any caching
	//         - This will allow users to precompile pipelines and save the VkPipelineCache 
	//   - Explicit render pass management
	//     - Not a render graph, but something that is a step below
	//     - User defines a pass in a readable fashion
	//     - Render pass is built and validated
	//     - 
	//   - Descriptors automatically managed
	//     - There may be a reason to support custom management
	//       - Think about this a tiny bit during architecting
	//     - Desc pools chained together once descriptors run out
	//     - Give user ability to control when descriptor sets are freed
	//       - Potentially have a meaningful default, but that may be difficult
	//       - Freeing descriptor sets may be a requirement of the library
	// Features of this library
	//   - Typed handles that represent the resources
	//     - Image vs ImageView would be separate handles
	//   - Direct management of the device via handle
	//     - Query information via device-based C api interface
	//   - Render Pass API
	//     - Validation of resource access within the pass
	//     - Subpass API
	//       - Need to investigate this....
	//   - Async compute support
	//     - No shit...
	// 
	//

    return 0;
}
