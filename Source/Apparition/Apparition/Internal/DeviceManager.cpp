
#include "DeviceManager.h"

#include "ApparitionInternals.h"
#include "HandleDefinitions.h"
#include "ImageFormatConversion.h"
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
	VK_EXT_SURFACE_MAINTENANCE_1_EXTENSION_NAME,
	VK_PLATFORM_SURFACE_EXTENSION,
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
	VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
	VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
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

//static void SetupDynamicRenderingFunctions(VkDevice device)
//{
//	vkCmdBeginRenderingKHR_ = (PFN_vkCmdBeginRenderingKHR)vkGetDeviceProcAddr(device, "vkCmdBeginRenderingKHR");
//	vkCmdEndRenderingKHR_ = (PFN_vkCmdEndRenderingKHR)vkGetDeviceProcAddr(device, "vkCmdEndRenderingKHR");
//}

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

static u32 initialPoolSize = 128;

DeviceManager::~DeviceManager()
{
	// Assert that there are no devices that still exist(?)
	// TODO - We have an initialize, we should have a deinitialize instead of having this destructor
	vkDestroyDebugUtilsMessengerEXT(instance, debugMessengerHandle, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void DeviceManager::Initialize(const Apparition::InitializeParams& params)
{
	InitializeFormatMapping();

	u32 instanceVersion;
	vkEnumerateInstanceVersion(&instanceVersion);
//	instanceVersion = instanceVersion & 0xFFFFF000;
	NOT_USED u32 minorVersion = VK_API_VERSION_MINOR(instanceVersion);
	NOT_USED u32 patchVersion = VK_API_VERSION_PATCH(instanceVersion);

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

	// Disables Shader Validation Feature, due to warning
	VkValidationFeatureDisableEXT disabledFeatures[] = { VkValidationFeatureDisableEXT::VK_VALIDATION_FEATURE_DISABLE_SHADER_VALIDATION_CACHE_EXT };
	VkValidationFeaturesEXT validationFeatures = {};
	validationFeatures.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
	validationFeatures.disabledValidationFeatureCount = ArraySize(disabledFeatures);
	validationFeatures.pDisabledValidationFeatures = disabledFeatures;

	VkInstanceCreateInfo instanceInfo;
	Vk::ZeroInfoStruct(instanceInfo, VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO);
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledLayerCount = (u32)ArraySize(validationLayers);
	instanceInfo.ppEnabledLayerNames = validationLayers;
	instanceInfo.enabledExtensionCount = (u32)ArraySize(instanceExtensions);
	instanceInfo.ppEnabledExtensionNames = instanceExtensions;
	//instanceInfo.pNext = &validationFeatures;

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

Apparition::Device DeviceManager::CreateDevice(const Apparition::DeviceCreationParams& params)
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

	u32 graphicsQueueCount = 0;
	u32 transferQueueCount = 0;
	NOT_USED u32 computeQueueCount = 0;
	DynamicArray<VkDeviceQueueCreateInfo> queueInfos;
	if (params.queueCreationCallback.IsValid())
	{
		u32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(selectedGpu, &queueFamilyCount, nullptr);
		Assert(queueFamilyCount > 0);
		DynamicArray<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		queueInfos = params.queueCreationCallback(queueFamilyProperties, graphicsFamilyIndex, transferFamilyIndex, computeFamilyIndex);
		Assertf(false, "We currently don't support custom queue setup due to the handle pooling system. This will be supported soon");
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
		graphicsQueueCount = 1;
		if (params.transferSupport)
		{
			queueInfo.queueFamilyIndex = transferFamilyIndex;
			queueInfos.Add(queueInfo);
			transferQueueCount = 1;
		}
		if (params.computeSupport)
		{
			if (!params.graphicsSupport || computeFamilyIndex != graphicsFamilyIndex)
			{
				queueInfo.queueFamilyIndex = computeFamilyIndex;
				queueInfos.Add(queueInfo);
				computeQueueCount = 1;
			}
		}
	}

	const tchar* deviceExtensions[] = {
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, // Eliminates the need for render pass begin/end
		VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
		VK_KHR_MAINTENANCE_5_EXTENSION_NAME,
		VK_EXT_DESCRIPTOR_HEAP_EXTENSION_NAME,
		VK_KHR_SWAPCHAIN_EXTENSION_NAME, // swapchain support
		//VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME, // TODO - Reenable and adhere to this functionality
		VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME, // Special semaphores that can replace VkSemaphore and VkFence
		VK_KHR_UNIFORM_BUFFER_STANDARD_LAYOUT_EXTENSION_NAME,
		VK_EXT_HOST_IMAGE_COPY_EXTENSION_NAME,
		VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME
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

	DeviceInternal internalDevice = {
		.physicalDevice = selectedGpu,
		.graphicsFamilyIndex = graphicsFamilyIndex,
		.transferFamilyIndex = transferFamilyIndex,
		.computeFamilyIndex = computeFamilyIndex
	};

	// Initialize dynamic rendering extension
	VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature;
	Vk::ZeroInfoStruct(dynamicRenderingFeature, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR);
	dynamicRenderingFeature.dynamicRendering = VK_TRUE;

	// Initialize synchronization2 features
	VkPhysicalDeviceSynchronization2Features synchronization2Feature;
	Vk::ZeroInfoStruct(synchronization2Feature, VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES);
	synchronization2Feature.synchronization2 = VK_TRUE;
	synchronization2Feature.pNext = &dynamicRenderingFeature;

	// Initialize buffer device address features
	VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
		.pNext = &synchronization2Feature,
		.bufferDeviceAddress = VK_TRUE
	};

	// Initialize descriptor heap features
	VkPhysicalDeviceDescriptorHeapFeaturesEXT descriptorHeapFeatures = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_HEAP_FEATURES_EXT,
		.pNext = &bufferDeviceAddressFeatures,
		.descriptorHeap = VK_TRUE
	};

	VkPhysicalDeviceFeatures2 supportedGpuFeatures;
	supportedGpuFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	supportedGpuFeatures.pNext = &descriptorHeapFeatures;
	vkGetPhysicalDeviceFeatures2(internalDevice.physicalDevice, &supportedGpuFeatures);

	//VkPhysicalDeviceFeatures enabledDeviceFeatures;
	/*
	if (params.featureSetupCallback.IsValid())
	{
		params.featureSetupCallback(supportedGpuFeatures, enabledDeviceFeatures);
	}
	//*/

	VkDeviceCreateInfo deviceInfo;
	Vk::ZeroInfoStruct(deviceInfo, VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO);
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.queueCreateInfoCount = queueInfos.Size();
	deviceInfo.pQueueCreateInfos = queueInfos.GetData();
	deviceInfo.enabledExtensionCount = deviceExtensionCount;
	deviceInfo.ppEnabledExtensionNames = deviceExtensions;
	deviceInfo.pEnabledFeatures = nullptr;//&enabledDeviceFeatures;
	deviceInfo.pNext = &supportedGpuFeatures;
	result = vkCreateDevice(selectedGpu, &deviceInfo, nullptr, &internalDevice.device);
	CHECK_VK(result);

	{
		internalDevice.graphicsQueueHandlePool = CreateHandlePool(graphicsQueueCount);
		internalDevice.graphicsQueues.Resize(graphicsQueueCount);
	}
	{
		internalDevice.transferQueueHandlePool = CreateHandlePool(transferQueueCount);
		internalDevice.transferQueues.Resize(transferQueueCount);
	}

	// We want zero to be reserved, since that's the "invalid handle" value
	u64 handleIndex = deviceInternals.Size() + 1;
	u64 handleValue = ((NextDeviceHandle++) << DEVICE_INDEX_SHIFT) | (handleIndex & RESOURCE_INDEX_MASK);
	Apparition::Device NewDeviceHandle{
		.handle = handleValue
	};
	InitializeDeviceHandlePools(internalDevice);

	const VmaAllocatorCreateInfo allocatorCreateInfo{
		//.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT,
		.physicalDevice = internalDevice.physicalDevice,
		.device = internalDevice.device,
		.instance = instance,
		.vulkanApiVersion = VK_API_VERSION_1_3
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

	deviceInternals.Add(internalDevice);
	// Sets up extention functionality
	//SetupDynamicRenderingFunctions(internalDevice.device);

	return NewDeviceHandle;
}

void DeviceManager::DestroyDevice(Apparition::Device deviceHandle)
{
	UNUSED(deviceHandle);
}

DeviceInternal& DeviceManager::GetDeviceInternals(Apparition::Device deviceHandle)
{
	Assert(deviceHandle.handle != Apparition::InvalidHandle);
	// Adjust for incremented index when handle was created
	u32 deviceIndex = (deviceHandle.handle & RESOURCE_INDEX_MASK);
	return GetDeviceInternals(deviceIndex);
}

DeviceInternal& DeviceManager::GetDeviceInternals(u32 deviceIndex)
{
	deviceIndex -= 1;
	Assert(deviceInternals.IsIndexValid(deviceIndex));
	return deviceInternals[deviceIndex];
}

void DeviceManager::InitializeDeviceHandlePools(DeviceInternal& deviceInternal)
{
	{
		HandlePool& commandPoolHandlePool = deviceInternal.commandPoolsHandlePool;
		Assert(commandPoolHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.commandPoolsHandlePool = CreateHandlePool(initialPoolSize);

		deviceInternal.commandPools.Resize(initialPoolSize);
	}

	{
		HandlePool& commandBufferHandlePool = deviceInternal.commandBufferHandlePool;
		Assert(commandBufferHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.commandBufferHandlePool = CreateHandlePool(initialPoolSize);
		
		deviceInternal.commandBuffers.Resize(initialPoolSize);
	}

	{
		HandlePool& bufferResourceHandlePool = deviceInternal.bufferResourceHandlePool;
		Assert(bufferResourceHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.bufferResourceHandlePool = CreateHandlePool(initialPoolSize);

		deviceInternal.bufferResources.Resize(initialPoolSize);
	}

	constexpr u32 extraSwapchainImages = 3;
	{
		HandlePool& imageResourceHandlePool = deviceInternal.imageResourceHandlePool;
		Assert(imageResourceHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.imageResourceHandlePool = CreateHandlePool(initialPoolSize + extraSwapchainImages);

		deviceInternal.imageResources.Resize(initialPoolSize + extraSwapchainImages);
	}

	{
		HandlePool& imageViewResourceHandlePool = deviceInternal.imageViewResourceHandlePool;
		Assert(imageViewResourceHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.imageViewResourceHandlePool = CreateHandlePool(initialPoolSize + extraSwapchainImages);

		deviceInternal.imageViewResources.Resize(initialPoolSize + extraSwapchainImages);
	}
}

Queue DeviceManager::AllocateGraphicsQueue(Device device)
{
	DeviceInternal& deviceInternal = GetDeviceInternals(device);
	Assert(deviceInternal.graphicsQueues.Size() > 0);
	u32 handleIndex = PopFreeHandleIndex(deviceInternal.graphicsQueueHandlePool);
	if (handleIndex != InvalidHandleIndex)
	{
		const u32 queueIndex = handleIndex - 1;
		VkQueue queue = VK_NULL_HANDLE;
		vkGetDeviceQueue(deviceInternal.device, deviceInternal.graphicsFamilyIndex, queueIndex, &queue);

		QueueInternal queueInternal{
			.queue = queue
		};
		deviceInternal.graphicsQueues[queueIndex] = queueInternal;

		const u32 indexGeneration = GetHandleGeneration(deviceInternal.graphicsQueueHandlePool, handleIndex);
		// TODO(nblane): this MUST be moved so that it can be reused
		const u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
			| (((u64)deviceInternal.graphicsFamilyIndex) << POOL_INDEX_SHIFT)
			| (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
			| (handleIndex & RESOURCE_INDEX_MASK);
		return Queue{ handleData };
	}

	return { InvalidHandle };
}

Queue DeviceManager::AllocateTransferQueue(Device device)
{
	DeviceInternal& deviceInternal = GetDeviceInternals(device);
	Assert(deviceInternal.transferQueues.Size() > 0);
	const u32 handleIndex = PopFreeHandleIndex(deviceInternal.transferQueueHandlePool);
	if (handleIndex != InvalidHandleIndex)
	{
		const u32 queueIndex = handleIndex - 1;
		VkQueue queue = VK_NULL_HANDLE;
		vkGetDeviceQueue(deviceInternal.device, deviceInternal.transferFamilyIndex, queueIndex, &queue);

		QueueInternal queueInternal{
			.queue = queue
		};
		deviceInternal.transferQueues[queueIndex] = queueInternal;

		const u32 indexGeneration = GetHandleGeneration(deviceInternal.transferQueueHandlePool, handleIndex);
		// TODO(nblane): this MUST be moved so that it can be reused
		const u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
			| (((u64)deviceInternal.transferFamilyIndex) << POOL_INDEX_SHIFT)
			| (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
			| (handleIndex & RESOURCE_INDEX_MASK);
		return Queue{ handleData };
	}

	// Here we need to log an error, since all queues are taken at this point
	return { InvalidHandle };
}

// NOTE: Graphics and compute queues are the same queue as of right now. Having them be the same queue and support handles will happen soon
//Queue DeviceManager::AllocateComputeQueue(Device device)
//{
//	DeviceInternal& deviceInternal = GetDeviceInternals(device);
//	Assert(deviceInternal.computeQueues.Size() > 0);
//	u32 handleIndex = PopFreeHandleIndex(deviceInternal.computeQueueHandlePool);
//	if (handleIndex != InvalidHandleIndex)
//	{
//		const u32 queueIndex = handleIndex - 1;
//		VkQueue queue = VK_NULL_HANDLE;
//		vkGetDeviceQueue(deviceInternal.device, deviceInternal.computeFamilyIndex, queueIndex, &queue);
//
//		QueueInternal queueInternal{
//			.queue = queue
//		};
//		deviceInternal.computeQueues[queueIndex] = queueInternal;
//
//		const u32 indexGeneration = GetHandleGeneration(deviceInternal.computeQueueHandlePool, handleIndex);
//		// TODO(nblane): this MUST be moved so that it can be reused
//		const u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
//			| (((u64)deviceInternal.computeFamilyIndex) << POOL_INDEX_SHIFT)
//			| (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
//			| (handleIndex & RESOURCE_INDEX_MASK);
//		return Queue{ handleData };
//	}
//
//	return { InvalidHandle };
//}

HandlePool& DeviceManager::GetQueueHandlePool(DeviceInternal& deviceInternal, u32 queueFamilyIndex)
{
	if (queueFamilyIndex == deviceInternal.graphicsFamilyIndex)
	{
		return deviceInternal.graphicsQueueHandlePool;
	}
	else if (queueFamilyIndex == deviceInternal.transferFamilyIndex)
	{
		return deviceInternal.transferQueueHandlePool;
	}
	else if (queueFamilyIndex == deviceInternal.computeFamilyIndex)
	{
		return deviceInternal.computeQueueHandlePool;
	}
	else
	{
		Assertf(false, "Unknown queueFamilyIndex value: {}", queueFamilyIndex);
		return deviceInternal.graphicsQueueHandlePool;
	}
}

const DynamicArray<QueueInternal>& DeviceManager::GetQueueArray(const DeviceInternal& deviceInternal, u32 queueFamilyIndex) const
{
	if (queueFamilyIndex == deviceInternal.graphicsFamilyIndex)
	{
		return deviceInternal.graphicsQueues;
	}
	else if (queueFamilyIndex == deviceInternal.transferFamilyIndex)
	{
		return deviceInternal.transferQueues;
	}
	else if (queueFamilyIndex == deviceInternal.computeFamilyIndex)
	{
		return deviceInternal.computeQueues;
	}
	else
	{
		Assertf(false, "Unknown queueFamilyIndex value: {}", queueFamilyIndex);
		return deviceInternal.graphicsQueues;
	}
}

void DeviceManager::FreeQueue(Queue queue)
{
	// TODO: Assert handle is valid AND handle generation is correct
	u32 deviceIndex = GetDeviceIndexFromHandle(queue);
	DeviceInternal deviceInternal = deviceInternals[deviceIndex - 1];
	u32 queueFamilyIndex = GetResourcePoolIndexFromHandle(queue);
	HandlePool& queueHandlePool = GetQueueHandlePool(deviceInternal, queueFamilyIndex);
	const u32 handleIndex = GetHandleIndex(queue);
	PushFreedHandleIndex(queueHandlePool, handleIndex);
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

CommandPool DeviceManager::CreateCommandPool(Device deviceHandle, const CommandPoolCreationParams& params)
{
	DeviceInternal& deviceInternal = GetDeviceInternals(deviceHandle);

	VkCommandPoolCreateInfo createInfo;
	Vk::ZeroInfoStruct(createInfo, VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO);
	createInfo.queueFamilyIndex = params.queueIndex;
	createInfo.flags = params.canResetCommandBuffers ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0;
	VkCommandPool cmdPool;
	VkResult result = vkCreateCommandPool(deviceInternal.device, &createInfo, nullptr, &cmdPool);
	CHECK_VK(result);
	if (result == VK_SUCCESS)
	{
		CommandPoolInternal commandPoolInternal = {};
		commandPoolInternal.cmdPool = cmdPool;
		commandPoolInternal.queueFamilyIndex = params.queueIndex;
		u32 handleIndex = PopFreeHandleIndex(deviceInternal.commandPoolsHandlePool);
		if (handleIndex != InvalidHandleIndex)
		{
			// Set the internal index to be the current data
			// handleIndex is valid, so we want to point to the "previous" element
			deviceInternal.commandPools[handleIndex-1] = commandPoolInternal;

			u32 indexGeneration = GetHandleGeneration(deviceInternal.commandPoolsHandlePool, handleIndex);
			// TODO(nblane): this MUST be moved so that it can be reused
			u64 handleData = (deviceHandle.handle << DEVICE_INDEX_SHIFT) 
				| (((u64)indexGeneration) << RESOURCE_GEN_SHIFT) 
				| (handleIndex & RESOURCE_INDEX_MASK);
			return CommandPool{handleData};
		}
	}
	return {InvalidHandle};
}

void DeviceManager::DestroyCommandPool(CommandPool commandPoolHandle)
{
	// TODO: Assert handle is valid AND handle generation is correct
	u32 deviceIndex = GetDeviceIndexFromHandle(commandPoolHandle);
	DeviceInternal deviceInternal = deviceInternals[deviceIndex-1];
	u32 handleIndex = GetHandleIndex(commandPoolHandle);
	VkCommandPool cmdPool = deviceInternal.commandPools[handleIndex-1].cmdPool;
	vkDestroyCommandPool(deviceInternal.device, cmdPool, nullptr);

	// Let the handle pool know this handle is freed
	PushFreedHandleIndex(deviceInternal.commandPoolsHandlePool, handleIndex);
}

CommandBuffer DeviceManager::AllocateCommandBuffer(CommandPool commandPoolHandle, const CommandBufferAllocParams& params)
{
	// TODO: having to REMEMBER to subtract 1 from the index is error prone. It might be better to have
	// accessors to this for you when passing in the index that's from the handle
	// 
	// NOTE: there currently isn't a reason other than array management to have access to these arrays. It might
	// make sense to prevent this from being allowed outside of resizing/deallocation
	u32 deviceIndex = GetDeviceIndexFromHandle(commandPoolHandle);
	DeviceInternal& deviceInternal = deviceInternals[deviceIndex-1];
	u32 cmdPoolIndex = GetHandleIndex(commandPoolHandle);
	CommandPoolInternal& commandPoolInternal = deviceInternal.commandPools[cmdPoolIndex-1];

	VkCommandBufferAllocateInfo allocInfo;
	Vk::ZeroInfoStruct(allocInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO);
	allocInfo.commandBufferCount = 1;
	allocInfo.commandPool = commandPoolInternal.cmdPool;
	allocInfo.level = !params.isSecondary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	VkCommandBuffer cmdBuffer;
	VkResult result = vkAllocateCommandBuffers(deviceInternal.device, &allocInfo, &cmdBuffer);
	CHECK_VK(result);
	if (result == VK_SUCCESS)
	{
		CommandBufferInternal commandBufferInternal = {};
		commandBufferInternal.commandBuffer = cmdBuffer;
		u32 handleIndex = PopFreeHandleIndex(deviceInternal.commandBufferHandlePool);
		if (handleIndex != InvalidHandleIndex)
		{
			// Set the internal index to be the current data
			deviceInternal.commandBuffers[handleIndex-1] = commandBufferInternal;

			u32 indexGeneration = GetHandleGeneration(deviceInternal.commandBufferHandlePool, handleIndex);
			// TODO(nblane): this MUST be moved so that it can be reused
			u64 handleData = ((u64)deviceIndex << DEVICE_INDEX_SHIFT)
				| ((u64)cmdPoolIndex << POOL_INDEX_SHIFT)
				| (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
				| (handleIndex & RESOURCE_INDEX_MASK);
			return CommandBuffer{ handleData };
		}
	}
	return { InvalidHandle };
}

void DeviceManager::FreeCommandBuffer(CommandBuffer commandBufferHandle)
{
	// TODO: Assert handle is valid AND handle generation is correct
	u32 deviceIndex = GetDeviceIndexFromHandle(commandBufferHandle);
	DeviceInternal& deviceInternal = deviceInternals[deviceIndex-1];
	u32 cmdPoolIndex = GetResourcePoolIndexFromHandle(commandBufferHandle);
	CommandPoolInternal& commandPoolInternal = deviceInternal.commandPools[cmdPoolIndex-1];
	u32 cmdBufferIndex = GetHandleIndex(commandBufferHandle);
	CommandBufferInternal& commandBufferInternal = deviceInternal.commandBuffers[cmdBufferIndex-1];
	vkFreeCommandBuffers(deviceInternal.device, commandPoolInternal.cmdPool, 1, &commandBufferInternal.commandBuffer);

	// Let the handle pool know this handle is freed
	PushFreedHandleIndex(deviceInternal.commandBufferHandlePool, cmdBufferIndex);
}

void DeviceManager::ResetCommandBuffer(CommandBuffer commandBuffer)
{
	u32 deviceIndex = GetDeviceIndexFromHandle(commandBuffer);
	DeviceInternal& deviceInternal = deviceInternals[deviceIndex - 1];
	u32 cmdBufferIndex = GetHandleIndex(commandBuffer);
	CommandBufferInternal& commandBufferInternal = GetCommandBufferInternalFromIndex(deviceInternal, cmdBufferIndex);
	VkResult result = vkResetCommandBuffer(commandBufferInternal.commandBuffer, 0);
	CHECK_VK(result);
}

VkCommandBuffer DeviceManager::GetCommandBufferHandle(CommandBuffer cbHandle)
{
	u32 deviceIndex = GetDeviceIndexFromHandle(cbHandle);
	DeviceInternal& deviceInternal = deviceInternals[deviceIndex-1];
	u32 cmdBufferIndex = GetHandleIndex(cbHandle);
	CommandBufferInternal& commandBufferInternal = GetCommandBufferInternalFromIndex(deviceInternal, cmdBufferIndex);

	return commandBufferInternal.commandBuffer;
}
