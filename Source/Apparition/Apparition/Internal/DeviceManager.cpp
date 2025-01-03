
#include "DeviceManager.h"
#include "Utilities/Array.hpp"
#include "VulkanCreateInfos.h"

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

static VkBool32 VulkanDebugMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
)
{
	DeviceManager* deviceManager = reinterpret_cast<DeviceManager*>(pUserData);
	if (deviceManager)
	{
		if (deviceManager->IsDebugFunctionSet())
		{
			// Passing the debug checking onto the user
			return deviceManager->BroadcastDebugCallback(messageSeverity, messageType, pCallbackData, pUserData);
		}
	}

	// We want default behavior to be logged in case the user opts to not set callback
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
				//MUSA_ERR(VkValidation, " {} : VUID({}): {}", typeStr, pCallbackData->pMessageIdName, pCallbackData->pMessage);
			}
			else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				//MUSA_WARN(VkValidation, " {} : VUID({}): {}", typeStr, pCallbackData->pMessageIdName, pCallbackData->pMessage);
			}
			else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			{
				//MUSA_INFO(VkValidation, " {} : VUID({}): {}", typeStr, pCallbackData->pMessageIdName, pCallbackData->pMessage);
			}
			else // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
			{
				//MUSA_DEBUG(VkValidation, " {} : VUID({}): {}", typeStr, pCallbackData->pMessageIdName, pCallbackData->pMessage);
			}
		}
	}

	return false;
}

DeviceManager& DeviceManager::Get()
{
	static DeviceManager deviceManager;
	return deviceManager;
}

DeviceManager::DeviceManager()
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
		instanceExtensions, (u32)ArraySize(instanceExtensions));
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
	debugInfo.pUserData = this;
	debugInfo.pfnUserCallback = &VulkanDebugMessengerCallback;

	result = vkCreateDebugUtilsMessengerEXT(instance, &debugInfo, nullptr, &debugMessengerHandle);
	CHECK_VK(result);
}

Apparition::DeviceHandle DeviceManager::CreateDevice(const Apparition::DeviceCreationParams& params)
{
	u32 physicalDeviceCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
	CHECK_VK(result);
	DynamicArray<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.GetData());
	CHECK_VK(result);

	u32 graphicsFamilyIndex = std::numeric_limits<u32>::max();
	u32 transferFamilyIndex = std::numeric_limits<u32>::max();
	u32 computeFamilyIndex = std::numeric_limits<u32>::max();

	const auto IsGpuSuitable = [&params, &graphicsFamilyIndex, &transferFamilyIndex, &computeFamilyIndex](VkPhysicalDevice physicalDevice)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(physicalDevice, &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) // Only supported GPU type currently
			{
				// TODO - Check if checking version of GPU api is necessary

				// TODO - Check what limits are important for a gpu
				Assert(properties.limits.maxImageDimension2D >= 4096); // Look into this assert more, tied into above TODO

				u32 queueFamilyCount = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
				Assert(queueFamilyCount > 0);
				DynamicArray<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.GetData());

				for (u32 i = 0; i < queueFamilyCount; ++i)
				{
					const VkBool32 presentationSupported = vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, i);

					const u32 desiredQueues = params.computeSupport ?
						VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT : VK_QUEUE_GRAPHICS_BIT;
					if (queueFamilyProperties[i].queueCount > 0 &&
						queueFamilyProperties[i].queueFlags & desiredQueues)
					{
						if (graphicsFamilyIndex == std::numeric_limits<u32>::max() && presentationSupported)
						{
							graphicsFamilyIndex = i;
							if (params.computeSupport)
							{
								computeFamilyIndex = i;
							}
						}
					}

					if (params.transferSupport &&
						queueFamilyProperties[i].queueCount > 0 &&
						queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
					{
						if (transferFamilyIndex == std::numeric_limits<u32>::max())
						{
							transferFamilyIndex = i;
						}
					}
				}

				return true;
			}

			return false;
		};

	// Select GPU that supports everything we care about
	VkPhysicalDevice selectedGpu = VK_NULL_HANDLE;
	for (const auto& physicalDevice : physicalDevices)
	{
		if (IsGpuSuitable(physicalDevice))
		{
			selectedGpu = physicalDevice;
			break;
		}
	}

	if (graphicsFamilyIndex == 0)
	{
		// TODO- Assert and return
	}

	if (transferFamilyIndex == 0 && params.transferSupport)
	{
		// TODO- Assert and return
	}

	if (computeFamilyIndex == 0 && params.computeSupport)
	{
		// TODO- Assert and return
	}

	DynamicArray<VkDeviceQueueCreateInfo> queueInfos;
	if (params.queueCreationCallback.IsValid())
	{
		u32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(selectedGpu, &queueFamilyCount, nullptr);
		Assert(queueFamilyCount > 0);
		DynamicArray<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		queueInfos = params.queueCreationCallback(queueFamilyProperties, graphicsFamilyIndex, transferFamilyIndex, computeFamilyIndex);
	}
	else
	{
		f32 priorities[] = { 1.f };
		queueInfos.Reserve(3);
		queueInfos.Add(Vk::DeviceQueueInfo(graphicsFamilyIndex, 1, priorities));
		if (params.transferSupport)
		{
			queueInfos.Add(Vk::DeviceQueueInfo(transferFamilyIndex, 1, priorities));
		}
		if (params.computeSupport)
		{
			queueInfos.Add(Vk::DeviceQueueInfo(computeFamilyIndex, 1, priorities));
		}
	}

	const tchar* deviceExtensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	DeviceInternals internalDevice = {
		.physicalDevice = selectedGpu,
		.graphicsFamilyIndex = graphicsFamilyIndex,
		.transferFamilyIndex = transferFamilyIndex,
		.computeFamilyIndex = computeFamilyIndex
	};

	VkPhysicalDeviceFeatures supportedGpuFeatures;
	vkGetPhysicalDeviceFeatures(internalDevice.physicalDevice, &supportedGpuFeatures);
	VkPhysicalDeviceFeatures enabledDeviceFeatures;
	if (params.featureSetupCallback.IsValid())
	{
		params.featureSetupCallback(supportedGpuFeatures, enabledDeviceFeatures);
	}

	VkDeviceCreateInfo deviceInfo = Vk::DeviceInfo(queueInfos.GetData(), queueInfos.Size(), deviceExtensions, (u32)ArraySize(deviceExtensions), enabledDeviceFeatures);
	result = vkCreateDevice(selectedGpu, &deviceInfo, nullptr, &internalDevice.device);
	CHECK_VK(result);

	Apparition::DeviceHandle NewDeviceHandle{
		.Handle = NextDeviceHandle++
	};
	vulkanDeviceDataMap.Add(NewDeviceHandle.Handle, internalDevice);

	return NewDeviceHandle;
}

void DeviceManager::DestroyDevice(Apparition::DeviceHandle deviceHandle)
{
	UNUSED(deviceHandle);
}

bool DeviceManager::BroadcastDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
	VkDebugUtilsMessageTypeFlagsEXT messageType, 
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
	void* pUserData)
{
	return DebugCallback(messageSeverity, messageType, pCallbackData, pUserData);
}
