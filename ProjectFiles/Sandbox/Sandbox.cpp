// Sandbox.cpp : Defines the entry point for the application.
//
#include <windows.h>

#define VK_PLATFORM_SURFACE_EXTENSION VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/DynamicArray.hpp"
#include "File/DirectoryLocations.hpp"
#include "Logging/LogCore.hpp"
#include "Logging/LogFunctions.hpp"
#include "Logging/Sinks/DebugOutputWindowSink.hpp"
#include "Path/Path.hpp"
#include "Utilities/Array.hpp"

DEFINE_LOG_CHANNEL(VkValidation);

namespace Vk
{
VkInstanceCreateInfo InstanceInfo(
	const tchar* const* instanceLayers, u32 numLayers,
	const tchar* const* instanceExtensions, u32 numExtensions,
	const void* additionalData = nullptr
)
{
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	//#if M_DEBUG
	createInfo.pNext = additionalData;
	createInfo.enabledLayerCount = numLayers;
	createInfo.ppEnabledLayerNames = instanceLayers;
	//#else
	UNUSED(numLayers, instanceLayers);
	//#endif
	createInfo.enabledExtensionCount = numExtensions;
	createInfo.ppEnabledExtensionNames = instanceExtensions;
	return createInfo;
}
}

constexpr const tchar* validationLayers[] = {
	"VK_LAYER_KHRONOS_validation",
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

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE /*hInstance*/,
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

	VkInstanceCreateInfo instanceInfo = Vk::InstanceInfo(validationLayers, (u32)ArraySize(validationLayers),
		instanceExtensions, (u32)ArraySize(instanceExtensions)/*, &debugInfo*/);
	VkInstance instance = VK_NULL_HANDLE;
	NOT_USED VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
	CHECK_VK(result);

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

	VkDebugUtilsMessengerEXT debugMessengerHandle = VK_NULL_HANDLE;
	result = vkCreateDebugUtilsMessengerEXT(instance, &debugInfo, nullptr, &debugMessengerHandle);
	CHECK_VK(result);

    return 0;
}
