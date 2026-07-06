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
#include "Memory/MemoryCore.hpp"
#include "Path/Path.hpp"
#include "Utilities/Array.hpp"

// Apparition
#include "Apparition/ApparitionCore.h"
#include "Apparition/Backbuffer.h"
#include "Apparition/Buffer.h"
#include "Apparition/CommandBuffer.h"
#include "Apparition/CommandBufferCommands.h"
#include "Apparition/Device.h"
#include "Apparition/Pipeline.h"
#include "Apparition/Queue.h"

// Sandbox
#include "Window.h"

DEFINE_LOG_CHANNEL(VkValidation);

#define CHECK_VK(expression) Assert(expression == VK_SUCCESS)

//////////////////////////////////////////////////////

// Things that are needed for support
//   - Function-based api
//   - Explicit device creation and management by user of api
//   - Pipeline creation and caching
//     - What does this mean?
//       - Pipeline setup done via user via api
//       - Caching is available, but separate object must be created
//         - This will allow users to precompile pipelines and save the VkPipelineCache 
//   - No RenderPass support
//     - will only support Vulkan's Dynamic Rendering extension
//   - Descriptors will be managed by the user
//     - 
//   - Two methods of using descriptors will be supported
//     - DescriptorSets (default support) and Descriptor Heaps (VK_EXT_descriptor_heap)
//     - Set up by the user to determine which method they're using
//     - Both methods will not be allowed at the same time
//   - Descriptor Sets
//     - If user uses descriptor sets, pipeline setup requires pipeline layouts
//     - Pipeline layouts can be created per pipeline request, so no management (see VK_KHR_maintenance4)
//     - Descriptor Pool and Updating Descriptors will be managed by the user
//   - Descriptor Heaps
//     - Opt-in mechanism upon initialization of the API
//       - This forbids usage of DescriptorSet API
//       - Forces checks within other parts of the API (e.g. Pipeline Creation) to ensure adherance to dheap functionality
//     - Supports both "untyped" and "backwards compatible" descriptor heaps
//       - Back compat heaps require more data in pipeline creation for shader stages. Currently no way to confirm one or the other
//         - Because of this, it may only support untyped in the future (once I understand how it works)
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
	VkPipeline vertexInput = VK_NULL_HANDLE;
	VkPipeline preRaster = VK_NULL_HANDLE;
	VkPipeline fragShader = VK_NULL_HANDLE;
	VkPipeline fragOutput = VK_NULL_HANDLE;

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



void CreateDescriptorHeaps(Apparition::Device device, VkPhysicalDevice physicalDevice)
{
	struct Data
	{
		Vector3 test0;
		Vector3 test1;
		Vector3 test2;
		Vector3 test3;
	};

	VkDevice deviceHandle = Apparition::GetVulkanDevice(device);

	Apparition::BufferCreationParams creationParams{
		.usage = Apparition::BufferUsageFlagBits::UniformBuffer | Apparition::BufferUsageFlagBits::ShaderDeviceAddress,
		.size = sizeof(Data),
		.supportsMappedMemory = true
	};
	Apparition::Buffer uniformBuffer = Apparition::CreateBuffer(device, creationParams);
	VkBuffer bufferHandle = Apparition::GetVulkanHandle(uniformBuffer);

	// Get Buffer Device Address
	VkBufferDeviceAddressInfoKHR bufferDeviceAddressInfo{
		.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
		.buffer = bufferHandle
	};
	NOT_USED VkDeviceAddress bufferDeviceAddress = vkGetBufferDeviceAddress(deviceHandle, &bufferDeviceAddressInfo);

	// Descriptor heaps have varying offset, size and alignment requirements, so we store it's properties for later user
	VkPhysicalDeviceDescriptorHeapPropertiesEXT descriptorHeapProperties{};

	VkPhysicalDeviceProperties2 deviceProps2{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
	descriptorHeapProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_PROPERTIES_EXT;
	deviceProps2.pNext = &descriptorHeapProperties;
	vkGetPhysicalDeviceProperties2(physicalDevice, &deviceProps2);

	// There are two descriptor heap types, one for resources and the other for samplers. Can't use "Combined image sampler"
	
	// Sampler descriptor heap
	{
		VkDeviceSize samplerDescriptorSize = Align(descriptorHeapProperties.samplerDescriptorSize, descriptorHeapProperties.samplerDescriptorAlignment);

		NOT_USED const VkDeviceSize heapSizeSamplers = Align(samplerDescriptorSize * 2 + descriptorHeapProperties.minSamplerHeapReservedRange, 
			descriptorHeapProperties.samplerHeapAlignment);
		// Create buffer that is a descriptor heap
	}
}

MemoryBuffer LoadShader(const char* shaderFile)
{
	MemoryBuffer shaderCode;
	FileSystem::Handle vertHandle;
	if (FileSystem::OpenFile(vertHandle, shaderFile, FileMode::Read))
	{
		u64 fileSize = FileSystem::FileSize(vertHandle);
		MemoryBuffer file{ fileSize };
		if (FileSystem::ReadFile(vertHandle, file.GetData(), (u32)fileSize))
		{
			file.CopyTo(shaderCode);
		}

		FileSystem::CloseFile(vertHandle);
	}

	return shaderCode;
}



Apparition::Pipeline CreateBasicGraphicsPipeline(Apparition::Device deviceHandle, VkFormat swapchainFormat)
{
	UNUSED(swapchainFormat);

	/*
	VkDevice device = Apparition::GetVulkanDevice(deviceHandle);
	// Pipeline Layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
	VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipeline.vkPipelineLayout);
	CHECK_VK(result);
	//*/

	////////////////////////
	// Pipeline Creation
	////////////////////////

	// Dynamic Rendering Setup: Gets passed to output stage
	/*
	VkPipelineRenderingCreateInfoKHR pipelineRenderingCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
		.viewMask = 0,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &swapchainFormat,
		.depthAttachmentFormat = VK_FORMAT_UNDEFINED,
		.stencilAttachmentFormat = VK_FORMAT_UNDEFINED
	};
	//*/

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
		const MemoryBuffer vertShaderCode = LoadShader("vert.spv");
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

		/*
		// Dynamic State
		DynamicArray<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.Size());
		dynamicState.pDynamicStates = dynamicStates.GetData();

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

		

		VkShaderModuleCreateInfo shaderInfo{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = vertShaderCode.Size(),
			// Careful with this...
			.pCode = (const u32*)vertShaderCode.GetData()
		};

		// Shader Module
		VkPipelineShaderStageCreateInfo vertStageInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = &shaderInfo,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			//.module = vertexShaderModule, // Unused, hurray!
			.pName = "main"
		};

		VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
			//.pNext = &pipelineRenderingCreateInfo,
			.flags = VK_GRAPHICS_PIPELINE_LIBRARY_PRE_RASTERIZATION_SHADERS_BIT_EXT
		};

		VkGraphicsPipelineCreateInfo pipelineLibraryInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = &libraryInfo,
			.flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT,
			.stageCount = 1,
			.pStages = &vertStageInfo,
			.pViewportState = &viewportState,
			.pRasterizationState = &rasterizer,
			.pDynamicState = &dynamicState,
			.layout = pipeline.vkPipelineLayout
		};
		result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineLibraryInfo, nullptr, &pipeline.preRaster);
		CHECK_VK(result);
		//*/
	}

	// Fragment Shader
	Apparition::FragmentShaderPipelineState fragmentShader;
	{
		MemoryBuffer fragShaderCode = LoadShader("frag.spv");
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

		/*
		VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
			//.pNext = &pipelineRenderingCreateInfo,
			.flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_SHADER_BIT_EXT
		};

		// Depth/Stencil
		VkPipelineDepthStencilStateCreateInfo depthStencilState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_FALSE,
			.depthWriteEnable = VK_FALSE,
			.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL
		};

		// Multisampling
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		// Fragment shader
		

		VkShaderModuleCreateInfo shaderInfo{
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = fragShaderCode.Size(),
			.pCode = (const u32*)fragShaderCode.GetData(),
		};

		VkPipelineShaderStageCreateInfo fragStageInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = &shaderInfo,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			//.module = fragmentShaderModule,
			.pName = "main"
		};

		VkGraphicsPipelineCreateInfo pipelineLibraryInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = &libraryInfo,
			.flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT,
			.stageCount = 1,
			.pStages = &fragStageInfo,
			.pMultisampleState = &multisampling,
			.pDepthStencilState = &depthStencilState,
			.layout = pipeline.vkPipelineLayout
		};
		result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineLibraryInfo, nullptr, &pipeline.fragShader);
		CHECK_VK(result);
		//*/
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
				Apparition::ImageFormat::RGBA_8norm
			}
		};

		fragmentOutput = Apparition::CreateFragmentOutputPipelineState(deviceHandle, params);

		/*
		VkGraphicsPipelineLibraryCreateInfoEXT libraryInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT,
			.pNext = &pipelineRenderingCreateInfo,
			.flags = VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_OUTPUT_INTERFACE_BIT_EXT
		};

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

		// Multisampling
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		VkGraphicsPipelineCreateInfo pipelineLibraryInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = &libraryInfo,
			.flags = VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT,
			.pMultisampleState = &multisampling,
			.pColorBlendState = &colorBlending,
			//.layout = pipeline.vkPipelineLayout
		};
		result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineLibraryInfo, nullptr, &pipeline.fragOutput);
		CHECK_VK(result);
		//*/
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

	/*
	DynamicArray<VkPipeline> libraries{
		pipeline.vertexInput,
		pipeline.preRaster,
		pipeline.fragShader,
		pipeline.fragOutput
	};

	VkPipelineLibraryCreateInfoKHR pipelineLibraryInfo{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR,
		//.pNext = &pipelineRenderingCreateInfo,
		.libraryCount = libraries.Size(),
		.pLibraries = libraries.GetData()
	};

	// Graphics Pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &pipelineLibraryInfo,
		.layout = pipeline.vkPipelineLayout
	};

	result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.vkPipeline);
	CHECK_VK(result);
	//*/
}

extern bool windowOpen;

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

	Apparition::Pipeline pipeline = CreateBasicGraphicsPipeline(deviceHandle, (VkFormat)Apparition::GetBackbufferVkFormat(deviceHandle));

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

	while (windowOpen)
	{
		ProcessWindowInput();

		Apparition::BackbufferStatus preparationStatus = Apparition::AcquireBackbufferImage(deviceHandle);
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
			.loadStoreOps = Apparition::AttachmentOperations::Clear_Store,
			.clearValue = {{.5f, .5f, .5f, 1.f}}
		};

		const u32 backbufferWidth = Apparition::GetBackbufferWidth(deviceHandle);
		const u32 backbufferHeight = Apparition::GetBackbufferHeight(deviceHandle);

		Apparition::RenderSetupParams renderSetup = {};
		renderSetup.colorAttachments.Add(colorAttachment);
		renderSetup.renderWidth = backbufferWidth;
		renderSetup.renderHeight = backbufferHeight;
		Apparition::BeginRendering(cmdBufferHandle, renderSetup);

		Apparition::BindGraphicsPipeline(cmdBufferHandle, pipeline);

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
		Apparition::SubmitBackbufferCommandBuffer(cmdBufferHandle, graphicsQueue);
		Apparition::PresentBackbuffer(graphicsQueue);
	}

	Apparition::WaitForIdle(graphicsQueue);

	Apparition::DestroyPipeline(pipeline);

	Apparition::FreeCommandBuffer(cmdBufferHandle);
	Apparition::DestroyCommandPool(cmdPoolHandle);

	Apparition::DestroyBuffer(indexBufferHandle);
	Apparition::DestroyBuffer(vertexBufferHandle);

	Apparition::TeardownBackbuffer(deviceHandle);
	Apparition::DestroyDevice(deviceHandle);

    return 0;
}
