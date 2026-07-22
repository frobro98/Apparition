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

	InitializeBaseExample(deviceHandle);

	while (windowOpen)
	{
		ProcessWindowInput();

		TickBaseExample();
	}

	DestroyBaseExample();


	Apparition::TeardownBackbuffer(deviceHandle);
	Apparition::DestroyDevice(deviceHandle);

    return 0;
}
