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

// Sandbox
#include "Window.h"

DEFINE_LOG_CHANNEL(VkValidation);

namespace Vk
{
VkInstanceCreateInfo InstanceInfo(
	const VkApplicationInfo& appInfo,
	const tchar* const* instanceLayers, u32 numLayers,
	const tchar* const* instanceExtensions, u32 numExtensions
)
{
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	//#if M_DEBUG
	createInfo.enabledLayerCount = numLayers;
	createInfo.ppEnabledLayerNames = instanceLayers;
	//#else
	UNUSED(numLayers, instanceLayers);
	//#endif
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = numExtensions;
	createInfo.ppEnabledExtensionNames = instanceExtensions;
	return createInfo;
}

VkDeviceQueueCreateInfo DeviceQueueInfo(u32 queueFamilyIndex, u32 numQueues, const f32* queuePriorities)
{
	VkDeviceQueueCreateInfo queueInfo = {};
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.queueFamilyIndex = queueFamilyIndex;
	queueInfo.queueCount = numQueues;
	queueInfo.pQueuePriorities = queuePriorities;
	return queueInfo;
}

VkDeviceCreateInfo DeviceInfo(const VkDeviceQueueCreateInfo* queueInfo, u32 numQueueInfos, const tchar** deviceExtensions, u32 numExtensions, const VkPhysicalDeviceFeatures& deviceFeatures)
{
	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.queueCreateInfoCount = numQueueInfos;
	deviceInfo.pQueueCreateInfos = queueInfo;
	deviceInfo.enabledExtensionCount = numExtensions;
	deviceInfo.ppEnabledExtensionNames = deviceExtensions;
	deviceInfo.pEnabledFeatures = &deviceFeatures;
	return deviceInfo;
}

VkWin32SurfaceCreateInfoKHR SurfaceInfo(HINSTANCE instance, HWND wnd)
{
	VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceInfo.hinstance = instance;
	surfaceInfo.hwnd = wnd;
	return surfaceInfo;
}

VkImageViewCreateInfo ImageViewInfo(VkImage image, u32 mipLevels, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo imageViewInfo = {};
	imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewInfo.image = image;
	imageViewInfo.format = format;
	imageViewInfo.subresourceRange.aspectMask = aspectFlags;
	imageViewInfo.subresourceRange.baseArrayLayer = 0;
	imageViewInfo.subresourceRange.layerCount = 1;
	imageViewInfo.subresourceRange.baseMipLevel = 0;
	imageViewInfo.subresourceRange.levelCount = mipLevels;
	return imageViewInfo;
}
}

constexpr const tchar* validationLayers[] = {
	"VK_LAYER_KHRONOS_validation",
	//"VK_LAYER_LUNARG_api_dump",
	//"VK_LAYER_LUNARG_object_tracker"
	//, "VK_LAYER_LUNARG_standard_validation"
	//, "VK_LAYER_LUNARG_parameter_validation"
	//, "VK_LAYER_GOOGLE_threading"
	//, "VK_LAYER_GOOGLE_unique_objects"
};

constexpr const tchar* instanceExtensions[] = {
	VK_KHR_SURFACE_EXTENSION_NAME,
	VK_PLATFORM_SURFACE_EXTENSION,
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};

#define CHECK_VK(expression) Assert(expression == VK_SUCCESS)

static PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT_ = nullptr;
#define vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT_

static PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT_ = nullptr;
#define vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT_

static PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT_ = nullptr;
#define vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT_

static PFN_vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT_ = nullptr;
#define vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT_

static PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT_ = nullptr;
#define vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT_

static PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT_ = nullptr;
#define vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT_

static PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT_ = nullptr;
#define vkQueueInsertDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT_

static PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT_ = nullptr;
#define vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT_

static PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT_ = nullptr;
#define vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT_

static PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT_ = nullptr;
#define vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT_

static void SetupDebugUtilsFunctions(VkInstance instance)
{
	vkCreateDebugUtilsMessengerEXT_ = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	vkDestroyDebugUtilsMessengerEXT_ = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	vkSetDebugUtilsObjectNameEXT_ = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");
	vkSetDebugUtilsObjectTagEXT_ = (PFN_vkSetDebugUtilsObjectTagEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectTagEXT");
	vkQueueBeginDebugUtilsLabelEXT_ = (PFN_vkQueueBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkQueueBeginDebugUtilsLabelEXT");
	vkQueueEndDebugUtilsLabelEXT_ = (PFN_vkQueueEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkQueueEndDebugUtilsLabelEXT");
	vkQueueInsertDebugUtilsLabelEXT_ = (PFN_vkQueueInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkQueueInsertDebugUtilsLabelEXT");
	vkCmdBeginDebugUtilsLabelEXT_ = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT");
	vkCmdEndDebugUtilsLabelEXT_ = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT");
	vkCmdInsertDebugUtilsLabelEXT_ = (PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdInsertDebugUtilsLabelEXT");
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

static VkBool32 VulkanDebugMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
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

			if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			{
				MUSA_ERR(VkValidation, " {} : VUID({}): {}", typeStr, pCallbackData->pMessageIdName, pCallbackData->pMessage);
			}
			else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				MUSA_WARN(VkValidation, " {} : VUID({}): {}", typeStr, pCallbackData->pMessageIdName, pCallbackData->pMessage);
			}
			else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
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

bool IsSuitableGpu(VkPhysicalDevice physicalDevice, u32& graphicsIndex, u32& transferIndex)
{
	VkPhysicalDeviceProperties deviceProperties;

	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		// TODO - Check if checking version of GPU api is necessary

		// TODO - Check what limits are important for a gpu
		Assert(deviceProperties.limits.maxImageDimension2D >= 4096);

		u32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		Assert(queueFamilyCount > 0);
		DynamicArray<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.GetData());

		for (u32 i = 0; i < queueFamilyCount; ++i)
		{
			VkBool32 presentationSupported = vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, i);

			if (queueFamilyProperties[i].queueCount > 0 &&
				queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
				queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				if (graphicsIndex == std::numeric_limits<u32>::max())
				{
					graphicsIndex = i;
				}

				if (presentationSupported)
				{
					graphicsIndex = i;
				}
			}
			else if (queueFamilyProperties[i].queueCount > 0 &&
				queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				if (transferIndex == std::numeric_limits<u32>::max())
				{
					transferIndex = i;
				}
			}
		}

		return true;
	}
	else
	{
		// TODO - Log that there isn't support for any other type of GPU

		return false;
	}
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

struct Device
{
	VkDevice vkDevice = VK_NULL_HANDLE;
	VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;

	VkQueue vkGraphicsQueue = VK_NULL_HANDLE;
	u32 graphicsQueueFamilyIndex = 0;
	VkQueue vkTransferQueue = VK_NULL_HANDLE;
	u32 transferQueueFamilyIndex = 0;

	VkCommandPool vkGraphicsCmdPool = VK_NULL_HANDLE;
	VkCommandPool vkTransferCmdPool = VK_NULL_HANDLE;

	VmaAllocator allocator;

	VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;
};

// Data will be accessible via handle
struct Surface
{
	VkSurfaceKHR vkSurface = VK_NULL_HANDLE;
	void* wndHandle = nullptr;
};

struct Image
{
	VkImage vkImage = VK_NULL_HANDLE;
	u32 width = 0;
	u32 height = 0;
	VkFormat format = VK_FORMAT_UNDEFINED;
	VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
};

struct ImageView
{
	VkImageView vkImageView = VK_NULL_HANDLE;
};

struct VertexBuffer
{
	VkBuffer vkBuffer = VK_NULL_HANDLE;
	VmaAllocation vmaAllocation = VK_NULL_HANDLE;
};

struct IndexBuffer
{
	VkBuffer vkBuffer = VK_NULL_HANDLE;
	VmaAllocation vmaAllocation = VK_NULL_HANDLE;
};

struct StagingBuffer
{
	VkBuffer vkBuffer = VK_NULL_HANDLE;
	VmaAllocation vmaAllocation = VK_NULL_HANDLE;
};

struct RenderPass
{
	VkRenderPass vkRenderPass = VK_NULL_HANDLE;
};

// Can be reused based on specific parts of the create info
struct Pipeline
{
	VkPipeline vkPipeline = VK_NULL_HANDLE;
	// Independent of pipeline since it just describes the layout
	VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;
};

struct Swapchain
{
	DynamicArray<VkFramebuffer> framebuffers;
	DynamicArray<VkImageView> imageViews;

	VkSwapchainKHR vkSwapchain = VK_NULL_HANDLE;
	VkExtent2D extents = {};
	VkFormat format = VK_FORMAT_UNDEFINED;
	VkSemaphore isImageAvailable = VK_NULL_HANDLE;
	VkSemaphore hasRenderingFinished = VK_NULL_HANDLE;
};

struct CommandBuffer
{
	VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
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

static void CreateInstance(VkInstance& instance, VkDebugUtilsMessengerEXT& debugMessengerHandle)
{
	u32 instanceVersion;
	vkEnumerateInstanceVersion(&instanceVersion);

	u32 layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	DynamicArray<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.GetData());

	u32 extensionCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	DynamicArray<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.GetData());

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Apparition Sandbox";
	appInfo.applicationVersion = 0;
	appInfo.pEngineName = "Apparition";
	appInfo.engineVersion = 0;
	appInfo.apiVersion = VK_MAKE_VERSION(1, 2, 0);

	VkInstanceCreateInfo instanceInfo = Vk::InstanceInfo(appInfo, validationLayers, (u32)ArraySize(validationLayers),
		instanceExtensions, (u32)ArraySize(instanceExtensions)/*, &debugInfo*/);
	NOT_USED VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
	CHECK_VK(result);

	SetupDebugUtilsFunctions(instance);

	VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
	debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	//debugInfo.pUserData = this;
	debugInfo.pfnUserCallback = &VulkanDebugMessengerCallback;

	result = vkCreateDebugUtilsMessengerEXT(instance, &debugInfo, nullptr, &debugMessengerHandle);
	CHECK_VK(result);
}

static void CreateDevice(VkInstance instance, Device& device)
{
	u32 physicalDeviceCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
	CHECK_VK(result);
	DynamicArray<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.GetData());
	CHECK_VK(result);

	u32 graphicsFamilyIndex = std::numeric_limits<u32>::max();
	u32 transferFamilyIndex = std::numeric_limits<u32>::max();
	for (const auto& physicalDevice : physicalDevices)
	{
		if (IsSuitableGpu(physicalDevice, graphicsFamilyIndex, transferFamilyIndex))
		{
			device.vkPhysicalDevice = physicalDevice;
			break;
		}
	}

	Assert(graphicsFamilyIndex != std::numeric_limits<u32>::max());
	Assert(transferFamilyIndex != std::numeric_limits<u32>::max());

	f32 priorities[] = { 1.0f };
	VkDeviceQueueCreateInfo queueInfos[] =
	{
		Vk::DeviceQueueInfo(graphicsFamilyIndex, 1, priorities),
		Vk::DeviceQueueInfo(transferFamilyIndex, 1, priorities)
	};

	const tchar* deviceExtensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// Enabling features
	VkPhysicalDeviceFeatures gpuFeatures;
	VkFormatProperties properties;
	VkPhysicalDeviceProperties gpuProperties;
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceFormatProperties(device.vkPhysicalDevice, VK_FORMAT_R8G8B8_UNORM, &properties);
	vkGetPhysicalDeviceFeatures(device.vkPhysicalDevice, &gpuFeatures);
	vkGetPhysicalDeviceProperties(device.vkPhysicalDevice, &gpuProperties);
	vkGetPhysicalDeviceMemoryProperties(device.vkPhysicalDevice, &memoryProperties);

	VkPhysicalDeviceFeatures enabledGPUFeatures{};
	if (gpuFeatures.geometryShader)
	{
		enabledGPUFeatures.geometryShader = VK_TRUE;
	}
	if (gpuFeatures.tessellationShader)
	{
		enabledGPUFeatures.tessellationShader = VK_TRUE;
	}
	if (gpuFeatures.fillModeNonSolid)
	{
		enabledGPUFeatures.fillModeNonSolid = VK_TRUE;
	}
	if (gpuFeatures.textureCompressionBC)
	{
		enabledGPUFeatures.textureCompressionBC = VK_TRUE;
	}
	if (gpuFeatures.textureCompressionETC2)
	{
		enabledGPUFeatures.textureCompressionETC2 = VK_TRUE;
	}
	if (gpuFeatures.textureCompressionASTC_LDR)
	{
		enabledGPUFeatures.textureCompressionASTC_LDR = VK_TRUE;
	}

	Assert(enabledGPUFeatures.fillModeNonSolid);
	Assert(enabledGPUFeatures.tessellationShader);
	Assert(enabledGPUFeatures.geometryShader);
	Assert(enabledGPUFeatures.textureCompressionBC);

	VkDeviceCreateInfo deviceInfo = Vk::DeviceInfo(queueInfos, (u32)ArraySize(queueInfos), deviceExtensions, (u32)ArraySize(deviceExtensions), enabledGPUFeatures);
	result = vkCreateDevice(device.vkPhysicalDevice, &deviceInfo, nullptr, &device.vkDevice);
	CHECK_VK(result);

	vkGetDeviceQueue(device.vkDevice, graphicsFamilyIndex, 0, &device.vkGraphicsQueue);
	vkGetDeviceQueue(device.vkDevice, transferFamilyIndex, 0, &device.vkTransferQueue);
	device.graphicsQueueFamilyIndex = graphicsFamilyIndex;
	device.transferQueueFamilyIndex = transferFamilyIndex;

	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = device.graphicsQueueFamilyIndex;
	// TODO - Find out if there are any flags for creating command pools
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	result = vkCreateCommandPool(device.vkDevice, &cmdPoolInfo, nullptr, &device.vkGraphicsCmdPool);
	CHECK_VK(result);

	cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = device.transferQueueFamilyIndex;
	// TODO - Find out if there are any flags for creating command pools
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	result = vkCreateCommandPool(device.vkDevice, &cmdPoolInfo, nullptr, &device.vkTransferCmdPool);
	CHECK_VK(result);

	VmaAllocatorCreateInfo allocatorCreateInfo{};
	//allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
	allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
	allocatorCreateInfo.physicalDevice = device.vkPhysicalDevice;
	allocatorCreateInfo.device = device.vkDevice;
	allocatorCreateInfo.instance = instance;

	result = vmaCreateAllocator(&allocatorCreateInfo, &device.allocator);
	CHECK_VK(result);

	VkPhysicalDeviceLimits limits = gpuProperties.limits;

	VkDescriptorPoolSize poolSizes[8] = {};
	poolSizes[0].descriptorCount = 10000;//maxSamplerPoolSize;
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 10000;//maxUniformBufferPoolSize;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[2].descriptorCount = limits.maxDescriptorSetUniformBuffersDynamic;
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	poolSizes[3].descriptorCount = 10000;//maxStorageBufferPoolSize;
	poolSizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSizes[4].descriptorCount = limits.maxDescriptorSetStorageBuffersDynamic;
	poolSizes[4].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
	poolSizes[5].descriptorCount = 10000;// maxStorageImagePoolSize;
	poolSizes[5].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSizes[6].descriptorCount = 10000;// maxSampledImagePoolSize;
	poolSizes[6].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	poolSizes[7].descriptorCount = 10000;// maxInputAttachmentPoolSize;
	poolSizes[7].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = (u32)ArraySize(poolSizes);
	poolInfo.pPoolSizes = poolSizes;
	// TODO - This is a horrible allocation scheme and it holds onto the memory the entire time. Must be a lot more conservative with my pools...
	poolInfo.maxSets = 10000;//logicalDevice.GetDeviceLimits().maxBoundDescriptorSets;
	// TODO - Figure out what this flag specifically does
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	result = vkCreateDescriptorPool(device.vkDevice, &poolInfo, nullptr, &device.vkDescriptorPool);
	CHECK_VK(result);

}

static void CreateSurface(VkInstance vkInstance, void* hInstance, void* wndHandle, Surface& surface)
{
	VkWin32SurfaceCreateInfoKHR surfaceInfo = Vk::SurfaceInfo((HINSTANCE)hInstance, (HWND)wndHandle);
	VkResult result = vkCreateWin32SurfaceKHR(vkInstance, &surfaceInfo, nullptr, &surface.vkSurface);
	CHECK_VK(result);

	surface.wndHandle = wndHandle;
}

static void CreateSwapchain(const Device& device, const Surface& surface, u32 width, u32 height, Swapchain& swapchain)
{
	// Get Surface Information Using Device
	VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
	VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.vkPhysicalDevice, surface.vkSurface, &surfaceCapabilities);
	CHECK_VK(result);

	DynamicArray<VkSurfaceFormatKHR> surfaceFormats;
	u32 formatCount = 0;
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(device.vkPhysicalDevice, surface.vkSurface, &formatCount, nullptr);
	CHECK_VK(result);
	surfaceFormats.Resize(formatCount);
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(device.vkPhysicalDevice, surface.vkSurface, &formatCount, surfaceFormats.GetData());
	CHECK_VK(result);

	DynamicArray<VkPresentModeKHR> presentModes;
	u32 presentModeCount;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(device.vkPhysicalDevice, surface.vkSurface, &presentModeCount, nullptr);
	CHECK_VK(result);
	presentModes.Resize(presentModeCount);
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(device.vkPhysicalDevice, surface.vkSurface, &presentModeCount, presentModes.GetData());
	CHECK_VK(result);

	// Create Swapchain
	u32 swapchainImageCount = surfaceCapabilities.minImageCount + 1;
	if (surfaceCapabilities.maxImageCount > 0 &&
		swapchainImageCount > surfaceCapabilities.maxImageCount)
	{
		swapchainImageCount = surfaceCapabilities.maxImageCount;
	}

	// Finding optimal supported surface format
	VkSurfaceFormatKHR surfaceFormat = {};
	if (surfaceFormats.Size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		surfaceFormat = {
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		};
	}
	else
	{
		// TODO - Add for-each support to my arrays
		for (u32 i = 0; i < surfaceFormats.Size(); ++i)
		{
			if (surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM)
			{
				surfaceFormat = surfaceFormats[i];
				break;
			}
		}
	}

	if (surfaceFormat.format != VK_FORMAT_R8G8B8A8_UNORM)
	{
		surfaceFormat = surfaceFormats[0];
	}
	swapchain.format = surfaceFormat.format;

	// Set up swapchain extents
	swapchain.extents.width = surfaceCapabilities.currentExtent.width == 0xffffffff ? width : surfaceCapabilities.currentExtent.width;
	swapchain.extents.height = surfaceCapabilities.currentExtent.height == 0xffffffff ? height : surfaceCapabilities.currentExtent.height;

	// Set up swapchain usage
	VkImageUsageFlags usageFlags;
	if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	else
	{
		usageFlags = static_cast<VkImageUsageFlags>(-1);
	}

	// Setting up the surface transform. Mostly going to be using current transform
	VkSurfaceTransformFlagBitsKHR transformBits = surfaceCapabilities.currentTransform;

	// Set up presentation mode
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	for (u32 i = 0; i < presentModes.Size(); ++i)
	{
		// Only checking against the mode with lowest latency and still has V-Sync
		//if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		//{
		//	presentMode = presentModes[i];
		//	break;
		//}
		//if(presentModes[i] == VK_PRESENT_MODE_FIFO_KHR)
		//{
		//	presentMode = presentModes[i];
		//}
		if (presentModes[i] == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
		{
			presentMode = presentModes[i];
		}
		else if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			presentMode = presentModes[i];
			break;
		}
	}

	VkSwapchainCreateInfoKHR swapchainInfo = {};
	swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainInfo.surface = surface.vkSurface;
	swapchainInfo.minImageCount = swapchainImageCount;
	swapchainInfo.imageFormat = surfaceFormat.format;
	swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainInfo.imageExtent = swapchain.extents;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageUsage = usageFlags;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainInfo.queueFamilyIndexCount = 0;
	swapchainInfo.pQueueFamilyIndices = nullptr;
	swapchainInfo.preTransform = transformBits;
	swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainInfo.presentMode = presentMode;
	swapchainInfo.clipped = VK_TRUE;
	swapchainInfo.oldSwapchain = VK_NULL_HANDLE; // No need for this sandbox atm

	result = vkCreateSwapchainKHR(device.vkDevice, &swapchainInfo, nullptr, &swapchain.vkSwapchain);
	CHECK_VK(result);

	u32 imageCount;
	vkGetSwapchainImagesKHR(device.vkDevice, swapchain.vkSwapchain, &imageCount, nullptr);
	DynamicArray<VkImage> swapchainImages{ imageCount };
	vkGetSwapchainImagesKHR(device.vkDevice, swapchain.vkSwapchain, &imageCount, swapchainImages.GetData());

	swapchain.imageViews.Reserve(imageCount);
	for (VkImage image : swapchainImages)
	{
		constexpr u32 miplevel = 1;
		constexpr VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		VkImageViewCreateInfo viewCreateInfo = Vk::ImageViewInfo(image, miplevel, swapchain.format, aspectFlags);
		viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;

		VkImageView imageView = VK_NULL_HANDLE;
		result = vkCreateImageView(device.vkDevice, &viewCreateInfo, nullptr, &imageView);
		CHECK_VK(result);

		swapchain.imageViews.Add(imageView);
	}

	// Semaphore creation
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	result = vkCreateSemaphore(device.vkDevice, &semaphoreCreateInfo, nullptr, &swapchain.isImageAvailable);
	CHECK_VK(result);
	result = vkCreateSemaphore(device.vkDevice, &semaphoreCreateInfo, nullptr, &swapchain.hasRenderingFinished);
	CHECK_VK(result);
}

void CreateRenderPass(const Device& device, const Swapchain& swapchain, RenderPass& renderPass)
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchain.format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkResult result = vkCreateRenderPass(device.vkDevice, &renderPassInfo, nullptr, &renderPass.vkRenderPass);
	CHECK_VK(result);
}

void CreateBasicGraphicsPipeline(const Device& device, const RenderPass& renderPass, Pipeline& pipeline)
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
	VkResult result = vkCreateShaderModule(device.vkDevice, &shaderInfo, nullptr, &vertexShaderModule);

	shaderInfo.codeSize = fragShaderCode.Size();
	// Careful with this...
	shaderInfo.pCode = (const u32*)fragShaderCode.GetData();
	result = vkCreateShaderModule(device.vkDevice, &shaderInfo, nullptr, &fragmentShaderModule);

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
	result = vkCreatePipelineLayout(device.vkDevice, &pipelineLayoutInfo, nullptr, &pipeline.vkPipelineLayout);

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
	pipelineInfo.renderPass = renderPass.vkRenderPass;
	pipelineInfo.subpass = 0;

	// These will most likely never be used in reality
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	result = vkCreateGraphicsPipelines(device.vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.vkPipeline);
	CHECK_VK(result);
}

void CreateSwapchainFramebuffers(const Device& device, const RenderPass& renderpass, Swapchain& swapchain)
{
	for (VkImageView imageView : swapchain.imageViews)
	{
		VkImageView attachments[] = { imageView };

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.attachmentCount = (u32)ArraySize(attachments);
		framebufferCreateInfo.pAttachments = attachments;
		framebufferCreateInfo.renderPass = renderpass.vkRenderPass;
		framebufferCreateInfo.width = swapchain.extents.width;
		framebufferCreateInfo.height = swapchain.extents.height;
		framebufferCreateInfo.layers = 1;
		
		VkFramebuffer framebuffer = VK_NULL_HANDLE;
		VkResult result = vkCreateFramebuffer(device.vkDevice, &framebufferCreateInfo, nullptr, &framebuffer);
		CHECK_VK(result);

		swapchain.framebuffers.Add(framebuffer);
	}
}

void CreateCommandBuffer(const Device& device, CommandBuffer& commandBuffer)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = device.vkGraphicsCmdPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VkResult result = vkAllocateCommandBuffers(device.vkDevice, &allocInfo, &commandBuffer.vkCommandBuffer);
	CHECK_VK(result);
}

void CreateVertexBuffer(const Device& device, VertexBuffer& vertexBuffer)
{
	//VkBufferCreateInfo bufferInfo{};
	//bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	//bufferInfo.size = sizeof(vertices[0]) * vertices.Size();
	//bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	//bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	//VkResult result = vkCreateBuffer(device.vkDevice, &bufferInfo, nullptr, &vertexBuffer.vkBuffer);
	//CHECK_VK(result);

	//VkBufferMemoryRequirementsInfo2 memReqInfo = { VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2_KHR };
	//memReqInfo.buffer = vertexBuffer.vkBuffer;

	//VkMemoryDedicatedRequirementsKHR memDedicatedReq = { VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS_KHR };
	//VkMemoryRequirements2KHR memReq2 = { VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2_KHR };
	//memReq2.pNext = &memDedicatedReq;

	//vkGetBufferMemoryRequirements2KHR(device.vkDevice, &memReqInfo, &memReq2);

	// Vertex Buffer
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(vertices[0]) * vertices.Size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

	VkResult result = vmaCreateBuffer(device.allocator, &bufferInfo, &allocCreateInfo, &vertexBuffer.vkBuffer, &vertexBuffer.vmaAllocation, nullptr);
	CHECK_VK(result);

	// Copy to VB

	// Release staging buffer and command buffer
}

void CreateIndexBuffer(const Device& device, IndexBuffer& indexBuffer)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(vertices[0]) * vertices.Size();
	bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;

	VkResult result = vmaCreateBuffer(device.allocator, &bufferInfo, &allocCreateInfo, &indexBuffer.vkBuffer, &indexBuffer.vmaAllocation, nullptr);
	CHECK_VK(result);
}

void CreateStagingBuffer(const Device& device, StagingBuffer& stagingBuffer, VkDeviceSize bufferSize)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocCreateInfo = {};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

	VkResult result = vmaCreateBuffer(device.allocator, &bufferInfo, &allocCreateInfo, &stagingBuffer.vkBuffer, &stagingBuffer.vmaAllocation, nullptr);
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

	// Create Vulkan Instance
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessengerHandle = VK_NULL_HANDLE;
	CreateInstance(instance, debugMessengerHandle);

	// Create Device
	Device device{};
	CreateDevice(instance, device);

	Surface surface = {};
	CreateSurface(instance, hInstance, window->windowHandle, surface);

	VkBool32 presentationSupported = VK_FALSE;
	vkGetPhysicalDeviceSurfaceSupportKHR(device.vkPhysicalDevice, device.graphicsQueueFamilyIndex, surface.vkSurface, &presentationSupported);
	Assert(presentationSupported == VK_TRUE);

	Swapchain swapchain = {};
	CreateSwapchain(device, surface, windowWidth, windowHeight, swapchain);

	RenderPass renderPass = {};
	CreateRenderPass(device, swapchain, renderPass);

	Pipeline pipeline = {};
	CreateBasicGraphicsPipeline(device, renderPass, pipeline);

	CreateSwapchainFramebuffers(device, renderPass, swapchain);

	CommandBuffer commandBuffer = {};
	CreateCommandBuffer(device, commandBuffer);

	VertexBuffer vertexBuffer = {};
	IndexBuffer indexBuffer = {};
	CreateVertexBuffer(device, vertexBuffer);
	CreateIndexBuffer(device, indexBuffer);

	{
		// Copy verts
		StagingBuffer vertStagingBuffer;
		CreateStagingBuffer(device, vertStagingBuffer, sizeof(vertices[0]) * vertices.Size());

		void* data;
		VkResult result = vmaMapMemory(device.allocator, vertStagingBuffer.vmaAllocation, &data);
		CHECK_VK(result);

		Memcpy(data, vertices.internalData, vertices.Size() * sizeof(Vertex));

		vmaUnmapMemory(device.allocator, vertStagingBuffer.vmaAllocation);
		data = nullptr;

		CommandBuffer copyVerts;
		CreateCommandBuffer(device, copyVerts);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(copyVerts.vkCommandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = vertices.Size() * sizeof(Vertex);
		vkCmdCopyBuffer(copyVerts.vkCommandBuffer, vertStagingBuffer.vkBuffer, vertexBuffer.vkBuffer, 1, &copyRegion);

		vkEndCommandBuffer(copyVerts.vkCommandBuffer);
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &copyVerts.vkCommandBuffer;

		vkQueueSubmit(device.vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(device.vkGraphicsQueue);

		vkFreeCommandBuffers(device.vkDevice, device.vkGraphicsCmdPool, 1, &copyVerts.vkCommandBuffer);
		vmaDestroyBuffer(device.allocator, vertStagingBuffer.vkBuffer, vertStagingBuffer.vmaAllocation);
	}

	{
		// Copy indices
		StagingBuffer idxStagingBuffer;
		CreateStagingBuffer(device, idxStagingBuffer, sizeof(indices[0]) * indices.Size());

		void* data;
		VkResult result = vmaMapMemory(device.allocator, idxStagingBuffer.vmaAllocation, &data);
		CHECK_VK(result);

		Memcpy(data, indices.internalData, indices.Size() * sizeof(u16));

		vmaUnmapMemory(device.allocator, idxStagingBuffer.vmaAllocation);
		data = nullptr;

		CommandBuffer copyIndices;
		CreateCommandBuffer(device, copyIndices);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(copyIndices.vkCommandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = indices.Size() * sizeof(u16);
		vkCmdCopyBuffer(copyIndices.vkCommandBuffer, idxStagingBuffer.vkBuffer, indexBuffer.vkBuffer, 1, &copyRegion);

		vkEndCommandBuffer(copyIndices.vkCommandBuffer);
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &copyIndices.vkCommandBuffer;

		vkQueueSubmit(device.vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(device.vkGraphicsQueue);

		vkFreeCommandBuffers(device.vkDevice, device.vkGraphicsCmdPool, 1, &copyIndices.vkCommandBuffer);
		vmaDestroyBuffer(device.allocator, idxStagingBuffer.vkBuffer, idxStagingBuffer.vmaAllocation);
	}

	while (true)
	{
		constexpr u64 timout = UINT64_MAX;
		constexpr VkFence imageFence = VK_NULL_HANDLE;
		u32 imageIndex;
		VkResult result = vkAcquireNextImageKHR(device.vkDevice, swapchain.vkSwapchain, timout, swapchain.isImageAvailable, imageFence, &imageIndex);
		Assert(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR || result == VK_NOT_READY);

		vkResetCommandBuffer(commandBuffer.vkCommandBuffer, 0);

		// Begin Command Buffer
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		result = vkBeginCommandBuffer(commandBuffer.vkCommandBuffer, &beginInfo);
		CHECK_VK(result);

		// Begin Render Pass
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass.vkRenderPass;
		renderPassInfo.framebuffer = swapchain.framebuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain.extents;
		VkClearValue clearColor = { { {0.5f, 0.5f, 0.5f, 1.f} } };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;
		vkCmdBeginRenderPass(commandBuffer.vkCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer.vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.vkPipeline);

		const VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer.vkCommandBuffer, 0, 1, &vertexBuffer.vkBuffer, offsets);

		vkCmdBindIndexBuffer(commandBuffer.vkCommandBuffer, indexBuffer.vkBuffer, 0, VK_INDEX_TYPE_UINT16);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchain.extents.width);
		viewport.height = static_cast<float>(swapchain.extents.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer.vkCommandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchain.extents;
		vkCmdSetScissor(commandBuffer.vkCommandBuffer, 0, 1, &scissor);

		vkCmdDrawIndexed(commandBuffer.vkCommandBuffer, (u32)indices.Size(), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffer.vkCommandBuffer);

		result = vkEndCommandBuffer(commandBuffer.vkCommandBuffer);
		CHECK_VK(result);

		// Submit Command Buffer
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { swapchain.isImageAvailable };
		VkSemaphore signalSemaphores[] = { swapchain.hasRenderingFinished };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer.vkCommandBuffer;
		result = vkQueueSubmit(device.vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &swapchain.hasRenderingFinished;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain.vkSwapchain;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(device.vkGraphicsQueue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			//Recreate()
		}
		else if (result != VK_SUCCESS)
		{
			// TODO - Log
			Assert(false);
		}

		vkQueueWaitIdle(device.vkGraphicsQueue);
	}

	vkDeviceWaitIdle(device.vkDevice);

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
