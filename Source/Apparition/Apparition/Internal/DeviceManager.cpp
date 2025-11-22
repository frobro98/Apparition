
#include "DeviceManager.h"

#include "Utilities/Array.hpp"
#include "VulkanInfos.h"

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

	
	return false;
}

DeviceManager::~DeviceManager()
{
	// Assert that there are no devices that still exist(?)
	// TODO - We have an initialize, we should have a deinitialize instead of having this destructor
	vkDestroyDebugUtilsMessengerEXT(instance, debugMessengerHandle, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void DeviceManager::Initialize(const Apparition::InitializeParams& params)
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
	appInfo.pApplicationName = params.applicationName;
	appInfo.applicationVersion = params.applicationVersion;
	appInfo.pEngineName = params.engineName;
	appInfo.engineVersion = params.engineVersion;
	appInfo.apiVersion = params.vulkanAPIVersion;

	VkInstanceCreateInfo instanceInfo;
	Vk::ZeroInfoStruct(instanceInfo, VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO);
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = (u32)ArraySize(validationLayers);
	instanceInfo.ppEnabledLayerNames = validationLayers;
	instanceInfo.enabledExtensionCount = (u32)ArraySize(instanceExtensions);
	instanceInfo.ppEnabledExtensionNames = instanceExtensions;

	NOT_USED VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
	CHECK_VK(result);

	SetupDebugUtilsFunctions(instance);

	VkDebugUtilsMessengerCreateInfoEXT debugInfo;
	Vk::ZeroInfoStruct(debugInfo, VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT);
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

void DeviceManager::Deinitialize()
{
	// NOTE - This will be removed once we have actual deinit and not use the destructor...
	this->~DeviceManager();
}

Apparition::DeviceHandle DeviceManager::CreateDevice(const Apparition::DeviceCreationParams& params)
{
	u32 physicalDeviceCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
	CHECK_VK(result);
	DynamicArray<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.GetData());
	CHECK_VK(result);

	constexpr u32 invalidFamilyIndex = std::numeric_limits<u32>::max();
	u32 graphicsFamilyIndex = invalidFamilyIndex;
	u32 transferFamilyIndex = invalidFamilyIndex;
	u32 computeFamilyIndex = invalidFamilyIndex;

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

					const u32 desiredQueues = [&params]() -> u32
						{
							if (params.graphicsSupport)
							{
								return params.computeSupport ?
									VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT : VK_QUEUE_GRAPHICS_BIT;
							}
							else if (params.computeSupport)
							{
								return VK_QUEUE_COMPUTE_BIT;
							}

							return 0;
						}();

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
					else if (params.transferSupport &&
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

	if (graphicsFamilyIndex == invalidFamilyIndex && params.graphicsSupport)
	{
		// TODO- Assert and return
	}

	if (transferFamilyIndex == invalidFamilyIndex && params.transferSupport)
	{
		// TODO- Assert and return
	}

	if (computeFamilyIndex == invalidFamilyIndex && params.computeSupport)
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
		VkDeviceQueueCreateInfo queueInfo;
		Vk::ZeroInfoStruct(queueInfo, VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO);
		if (params.graphicsSupport)
		{
			queueInfo.queueFamilyIndex = graphicsFamilyIndex;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = priorities;
		}

		queueInfos.Reserve(3);
		queueInfos.Add(queueInfo);
		if (params.transferSupport)
		{
			queueInfo.queueFamilyIndex = transferFamilyIndex;
			queueInfos.Add(queueInfo);
		}
		if (params.computeSupport)
		{
			if (!params.graphicsSupport || computeFamilyIndex != graphicsFamilyIndex)
			{
				queueInfo.queueFamilyIndex = computeFamilyIndex;
				queueInfos.Add(queueInfo);
			}
		}
	}

	const tchar* deviceExtensions[] = {
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
		VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
		VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME,
		"VK_EXT_swapchain_maintenance1",
		"VK_EXT_host_image_copy",
		"VK_EXT_scalar_block_layout"
	};
	u32 deviceExtensionCount = ArraySize(deviceExtensions);

#if 1 // TODO - Find a specific kind of preprocessor flag that allows for validating that device extensions exist
	u32 deviceLayerCount;
	vkEnumerateDeviceLayerProperties(selectedGpu, &deviceLayerCount, nullptr);
	DynamicArray<VkLayerProperties> deviceLayers(deviceLayerCount);
	vkEnumerateDeviceLayerProperties(selectedGpu, &deviceLayerCount, deviceLayers.GetData());

	u32 extensionCount;
	vkEnumerateDeviceExtensionProperties(selectedGpu, nullptr, &extensionCount, nullptr);
	DynamicArray<VkExtensionProperties> defaultDeviceExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(selectedGpu, nullptr, &extensionCount, defaultDeviceExtensions.GetData());

	DynamicArray<const tchar*> availableDeviceExtensions;
	availableDeviceExtensions.Reserve(ArraySize(deviceExtensions));
	for (const tchar* extension : deviceExtensions)
	{
		if (defaultDeviceExtensions.FindFirstIndexUsing([extension](const VkExtensionProperties& prop) {
				return Strcmp(prop.extensionName, extension) == 0;
			}) >= 0)
		{
			availableDeviceExtensions.Add(extension);
		}
	}

	Memcpy(deviceExtensions, availableDeviceExtensions.GetData(), availableDeviceExtensions.SizeInBytes());
	deviceExtensionCount = availableDeviceExtensions.Size();
#endif

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

	VkDeviceCreateInfo deviceInfo;
	Vk::ZeroInfoStruct(deviceInfo, VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO);
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.queueCreateInfoCount = queueInfos.Size();
	deviceInfo.pQueueCreateInfos = queueInfos.GetData();
	deviceInfo.enabledExtensionCount = deviceExtensionCount;
	deviceInfo.ppEnabledExtensionNames = deviceExtensions;
	deviceInfo.pEnabledFeatures = &enabledDeviceFeatures;
	result = vkCreateDevice(selectedGpu, &deviceInfo, nullptr, &internalDevice.device);
	CHECK_VK(result);

	Apparition::DeviceHandle NewDeviceHandle{
		.handle = NextDeviceHandle++
	};
	vulkanDeviceDataMap.Add(NewDeviceHandle.handle, internalDevice);

	// Device's command pool
	VkCommandPoolCreateInfo cmdPoolInfo;
	Vk::ZeroInfoStruct(cmdPoolInfo, VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO);

	if (internalDevice.graphicsFamilyIndex != invalidFamilyIndex)
	{
		cmdPoolInfo.queueFamilyIndex = internalDevice.graphicsFamilyIndex;
		// TODO - Find out if there are any flags for creating command pools
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		result = vkCreateCommandPool(internalDevice.device, &cmdPoolInfo, nullptr, &internalDevice.graphicsCmdPool);
		CHECK_VK(result);
	}

	if (internalDevice.transferFamilyIndex != invalidFamilyIndex)
	{
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolInfo.queueFamilyIndex = internalDevice.transferFamilyIndex;
		// TODO - Find out if there are any flags for creating command pools
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		result = vkCreateCommandPool(internalDevice.device, &cmdPoolInfo, nullptr, &internalDevice.transferCmdPool);
		CHECK_VK(result);
	}

	if (internalDevice.computeFamilyIndex != invalidFamilyIndex)
	{
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolInfo.queueFamilyIndex = internalDevice.computeFamilyIndex;
		// TODO - Find out if there are any flags for creating command pools
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		result = vkCreateCommandPool(internalDevice.device, &cmdPoolInfo, nullptr, &internalDevice.computeCmdPool);
		CHECK_VK(result);
	}

	const VmaAllocatorCreateInfo allocatorCreateInfo{
		//.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT,
		.physicalDevice = internalDevice.physicalDevice,
		.device = internalDevice.device,
		.instance = instance,
		.vulkanApiVersion = VK_API_VERSION_1_2
	};
	result = vmaCreateAllocator(&allocatorCreateInfo, &internalDevice.allocator);
	CHECK_VK(result);

	VkPhysicalDeviceProperties gpuProperties;
	vkGetPhysicalDeviceProperties(internalDevice.physicalDevice, &gpuProperties);
	const VkPhysicalDeviceLimits limits = gpuProperties.limits;

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
	result = vkCreateDescriptorPool(internalDevice.device, &poolInfo, nullptr, &internalDevice.descriptorPool);
	CHECK_VK(result);

	return NewDeviceHandle;
}

void DeviceManager::DestroyDevice(Apparition::DeviceHandle deviceHandle)
{
	UNUSED(deviceHandle);
}

DeviceInternals* DeviceManager::GetDeviceInternals(Apparition::DeviceHandle deviceHandle)
{
	return vulkanDeviceDataMap.Find(deviceHandle.handle);
}

bool DeviceManager::BroadcastDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
	VkDebugUtilsMessageTypeFlagsEXT messageType, 
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
	void* pUserData)
{
	const ValidationSeverity severity = [messageSeverity]
	{
			switch (messageSeverity)
			{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				return ValidationSeverity::Verbose;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				return ValidationSeverity::Info;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				return ValidationSeverity::Warning;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				return ValidationSeverity::Error;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			default:
				return ValidationSeverity::None;
			}
	}();
	const Apparition::DebugMessageTypeFlags messageTypeFlags = messageType;
	return userValidation.delegate(severity, messageTypeFlags, pCallbackData, pUserData);
}
