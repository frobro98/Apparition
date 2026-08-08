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
#include "Window/Window.h"
#include "Examples/ExampleCore.h"

// Examples
#include "Examples/Base/Base.h"
#include "Examples/DescriptorSets/DescriptorSets.h"

DEFINE_LOG_CHANNEL(VkValidation);

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
//   - Async compute support
//     - No shit...
// 
//

// State of API notes
// - TODO - Need to have some sort of distinction between structs/handles and functions
// -- Need to move the Apparition namespace to Apr namespace(??)
// - TODO - Add lots of validation to internal API
// - TODO - Determine whether raw buffers should instead be VertexBuffer, IndexBuffer, UniformBuffer, etc.
// -- If buffers are like that, should images be like that too??
// -- Global buffer functionality?
// -- What does this give me and the API?
// - TODO - Maybe focus on bindless descriptors?
// -- This would mean that regular descriptors might not be supported?
// -- Shouldn't support everything

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

extern bool windowOpen;
Window* window = nullptr;

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

	const u32 windowWidth = 1280;
	const u32 windowHeight = 720;
	window = CreateSandboxWindow(hInstance, 0, 0, windowWidth, windowHeight);


	AptnInitializeParams initParams{
		.applicationName = "Apparition Sandbox",
		.engineName = "Apparition",
		.applicationVersion = 0,
		.engineVersion = 0,
		.vulkanAPIVersion = APPARITION_MAKE_VERSION(1,3,290)
	};

	AptnValidationDelegate debugCallback = &VulkanDebugMessengerCallback;
	Apparition::SetErrorLogCallback(MOVE(debugCallback), nullptr);
	Apparition::InitializeApparition(initParams);

	AptnDevice deviceHandle;
	{
		AptnDeviceCreationParams createParams{
			.queueCreationParams
			{
				AptnQueueCreationParams
				{
					.queueType = AptnQueueType::Graphics
				},
				AptnQueueCreationParams
				{
					.queueType = AptnQueueType::Compute
				},
				AptnQueueCreationParams
				{
					.queueType = AptnQueueType::Transfer
				}
			}
		};
		deviceHandle = Apparition::CreateDevice(createParams);
	}

	//InitializeBaseExample(deviceHandle);
	InitializeDescriptorSetsExample(deviceHandle);

	while (windowOpen)
	{
		ProcessWindowInput();

		//TickBaseExample();
		TickDescriptorSetsExample();
	}

	//DestroyBaseExample();
	DestroyDescriptorSetsExample();

	Apparition::TeardownBackbuffer(deviceHandle);
	Apparition::DestroyDevice(deviceHandle);

    return 0;
}
