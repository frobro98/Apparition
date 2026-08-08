
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

void DeviceManager::Initialize(const AptnInitializeParams& params)
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

AptnDevice DeviceManager::CreateDevice(const AptnDeviceCreationParams& params)
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

	u32 graphicsSupport = false;
	u32 computeSupport = false;
	u32 transferSupport = false;
	Assert(!params.queueCreationParams.IsEmpty());
	for (const AptnQueueCreationParams& queueParams : params.queueCreationParams)
	{
		if (queueParams.queueType == AptnQueueType::Graphics)
		{
			graphicsSupport = true;
		}
		else if (queueParams.queueType == AptnQueueType::Compute)
		{
			computeSupport = true;
		}
		else if (queueParams.queueType == AptnQueueType::Transfer)
		{
			transferSupport = true;
		}
	}

	const auto IsGpuSuitable = [&graphicsFamilyIndex, &transferFamilyIndex, &computeFamilyIndex,
								graphicsSupport, transferSupport, computeSupport](VkPhysicalDevice physicalDevice)
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

					const u32 desiredQueues = [=]() -> u32
						{
							if (graphicsSupport)
							{
								return computeSupport ?
									VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT : VK_QUEUE_GRAPHICS_BIT;
							}
							else if (computeSupport)
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
							if (computeSupport)
							{
								computeFamilyIndex = i;
							}
						}
					}
					else if (transferSupport &&
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

	if (graphicsFamilyIndex == invalidFamilyIndex && graphicsSupport)
	{
		// TODO- Assert and return
	}

	if (transferSupport)
	{
		if (transferFamilyIndex == invalidFamilyIndex)
		{

		}
		// TODO- Assert and return
	}

	if (computeFamilyIndex == invalidFamilyIndex && computeSupport)
	{
		// TODO- Assert and return
	}

	u32 graphicsQueueCount = 0;
	u32 transferQueueCount = 0;
	u32 computeQueueCount = 0;
	DynamicArray<VkDeviceQueueCreateInfo> queueInfos;

	f32 priorities[] = { 1.f };
	VkDeviceQueueCreateInfo queueInfo;
	Vk::ZeroInfoStruct(queueInfo, VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO);
	if (graphicsSupport)
	{
		queueInfo.queueFamilyIndex = graphicsFamilyIndex;
		queueInfo.queueCount = 1;
		queueInfo.pQueuePriorities = priorities;
	}

	queueInfos.Reserve(3);
	queueInfos.Add(queueInfo);
	graphicsQueueCount = 1;
	if (transferSupport)
	{
		queueInfo.queueFamilyIndex = transferFamilyIndex;
		queueInfos.Add(queueInfo);
		transferQueueCount = 1;
	}
	if (computeSupport)
	{
		if (!graphicsSupport || computeFamilyIndex != graphicsFamilyIndex)
		{
			queueInfo.queueFamilyIndex = computeFamilyIndex;
			queueInfos.Add(queueInfo);
			computeQueueCount = 1;
		}
	}

	const tchar* deviceExtensions[] = {
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, // Eliminates the need for render pass begin/end
		VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,  // Required for graphics pipeline library ext
		VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME, // Allows for segmented pipelines that can be reused
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

	// Initialize descriptor indexing features
	VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES
	};

	// Initialize dynamic rendering extension
	VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
		.pNext = &descriptorIndexingFeatures,
		.dynamicRendering = VK_TRUE
	};

	// Initialize synchronization2 features
	VkPhysicalDeviceSynchronization2Features synchronization2Feature = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
		.pNext = &dynamicRenderingFeature,
		.synchronization2 = VK_TRUE
	};

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

	// Initialize graphics pipeline library features
	VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT graphicsPipelineLibraryFeatures = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT,
		.pNext = &descriptorHeapFeatures,
		.graphicsPipelineLibrary = VK_TRUE
	};

	VkPhysicalDeviceFeatures2 supportedGpuFeatures;
	supportedGpuFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	supportedGpuFeatures.pNext = &graphicsPipelineLibraryFeatures;
	vkGetPhysicalDeviceFeatures2(internalDevice.physicalDevice, &supportedGpuFeatures);

	Assert(descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing);
	Assert(descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind);
	Assert(descriptorIndexingFeatures.shaderUniformBufferArrayNonUniformIndexing);
	Assert(descriptorIndexingFeatures.descriptorBindingUniformBufferUpdateAfterBind);
	Assert(descriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing);
	Assert(descriptorIndexingFeatures.descriptorBindingStorageBufferUpdateAfterBind);

	//VkPhysicalDeviceFeatures enabledDeviceFeatures;
	/*
	if (params.featureSetupCallback.IsValid())
	{
		params.featureSetupCallback(supportedGpuFeatures, enabledDeviceFeatures);
	}
	//*/

	VkPhysicalDeviceFeatures enabledFeatures{};
	enabledFeatures.samplerAnisotropy = VK_TRUE;
	supportedGpuFeatures.features = enabledFeatures;

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
		const u32 queueCount = graphicsQueueCount + transferQueueCount + computeQueueCount;
		internalDevice.queueHandlePool = CreateHandlePool(queueCount);
		internalDevice.queues.Resize(queueCount);
	}

	// We want zero to be reserved, since that's the "invalid handle" value
	u64 handleIndex = deviceInternals.Size() + 1;
	u64 handleValue = ((NextDeviceHandle++) << DEVICE_INDEX_SHIFT) | (handleIndex & RESOURCE_INDEX_MASK);
	AptnDevice NewDeviceHandle{
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
	//const VkPhysicalDeviceLimits limits = gpuProperties.limits;

	deviceInternals.Add(internalDevice);
	// Sets up extention functionality
	//SetupDynamicRenderingFunctions(internalDevice.device);

	return NewDeviceHandle;
}

void DeviceManager::DestroyDevice(AptnDevice deviceHandle)
{
	UNUSED(deviceHandle);
}

DeviceInternal& DeviceManager::DeviceInternalFrom(AptnDevice deviceHandle)
{
	Assert(IsValid(deviceHandle));
	// Adjust for incremented index when handle was created
	u32 deviceIndex = (deviceHandle.handle & RESOURCE_INDEX_MASK);
	return DeviceInternalFrom(deviceIndex);
}

DeviceInternal& DeviceManager::DeviceInternalFrom(u32 deviceIndex)
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

	// Pipeline State Internals
	{
		HandlePool& vertexInputResourceHandlePool = deviceInternal.vertexInputResourceHandlePool;
		Assert(vertexInputResourceHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.vertexInputResourceHandlePool = CreateHandlePool(initialPoolSize);

		deviceInternal.vertexInputResources.Resize(initialPoolSize);
	}

	{
		HandlePool& prerasterShadersResourceHandlePool = deviceInternal.prerasterShadersResourceHandlePool;
		Assert(prerasterShadersResourceHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.prerasterShadersResourceHandlePool = CreateHandlePool(initialPoolSize);

		deviceInternal.prerasterShadersResources.Resize(initialPoolSize);
	}

	{
		HandlePool& fragmentShaderResourceHandlePool = deviceInternal.fragmentShaderResourceHandlePool;
		Assert(fragmentShaderResourceHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.fragmentShaderResourceHandlePool = CreateHandlePool(initialPoolSize);

		deviceInternal.fragmentShaderResources.Resize(initialPoolSize);
	}

	{
		HandlePool& fragmentOutputResourceHandlePool = deviceInternal.fragmentOutputResourceHandlePool;
		Assert(fragmentOutputResourceHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.fragmentOutputResourceHandlePool = CreateHandlePool(initialPoolSize);

		deviceInternal.fragmentOutputResources.Resize(initialPoolSize);
	}

	{
		HandlePool& pipelineResourceHandlePool = deviceInternal.pipelineResourceHandlePool;
		Assert(pipelineResourceHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.pipelineResourceHandlePool = CreateHandlePool(initialPoolSize);

		deviceInternal.pipelineResources.Resize(initialPoolSize);
	}

	// Image Resource Internals
	// 3 extra image/view resources to ensure access to the swapchain data
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

	// Sampler
	{
		HandlePool& samplerResourceHandlePool = deviceInternal.samplerResourceHandlePool;
		Assert(samplerResourceHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.samplerResourceHandlePool = CreateHandlePool(initialPoolSize);

		deviceInternal.samplerResources.Resize(initialPoolSize);
	}

	// Descriptor Set Internals
	{
		HandlePool& descriptorSetLayoutHandlePool = deviceInternal.descriptorSetLayoutHandlePools;
		Assert(descriptorSetLayoutHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.descriptorSetLayoutHandlePools = CreateHandlePool(initialPoolSize);

		deviceInternal.descriptorSetLayoutResources.Resize(initialPoolSize);
	}

	{
		HandlePool& descriptorPoolHandlePool = deviceInternal.descriptorPoolHandlePools;
		Assert(descriptorPoolHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.descriptorPoolHandlePools = CreateHandlePool(initialPoolSize);

		deviceInternal.descriptorPoolResources.Resize(initialPoolSize);
	}

	{
		HandlePool& descriptorSetHandlePool = deviceInternal.descriptorSetHandlePools;
		Assert(descriptorSetHandlePool.freeHandleIndices.IsEmpty());
		deviceInternal.descriptorSetHandlePools = CreateHandlePool(initialPoolSize);

		deviceInternal.descriptorSetResources.Resize(initialPoolSize);
	}
}

bool DeviceManager::CanAllocateQueue(const DeviceInternal& deviceInternal, u32 queueFamilyIndex) const
{
	for (const QueueInternal& queueInternal : deviceInternal.queues)
	{
		if (queueInternal.queue != VK_NULL_HANDLE && queueInternal.queueFamilyIndex == queueFamilyIndex)
		{
			// TODO - Right now, we only support one queue per type. More than that should be supported
			Assert(false);
			return false;
		}
	}

	return true;
}

AptnQueue DeviceManager::AllocateGraphicsQueue(AptnDevice device)
{
	DeviceInternal& deviceInternal = DeviceInternalFrom(device);
	if (CanAllocateQueue(deviceInternal, deviceInternal.graphicsFamilyIndex))
	{
		const u32 handleIndex = PopFreeHandleIndex(deviceInternal.queueHandlePool);
		if (handleIndex != InvalidHandleIndex)
		{
			constexpr u32 queueIndex = 0;
			VkQueue queue = VK_NULL_HANDLE;
			vkGetDeviceQueue(deviceInternal.device, deviceInternal.graphicsFamilyIndex, queueIndex, &queue);

			QueueInternal queueInternal{
				.queue = queue,
				.queueFamilyIndex = deviceInternal.graphicsFamilyIndex,
				.canPresent = true
			};
			
			GetQueueInternalFromIndex(deviceInternal, handleIndex) = queueInternal;

			const u32 indexGeneration = GetHandleGeneration(deviceInternal.queueHandlePool, handleIndex);
			// TODO(nblane): this MUST be moved so that it can be reused
			const u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
				| (((u64)deviceInternal.graphicsFamilyIndex) << POOL_INDEX_SHIFT)
				| (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
				| (handleIndex & RESOURCE_INDEX_MASK);
			return AptnQueue{ handleData };
		}
	}

	return { AptnInvalidHandle };
}

AptnQueue DeviceManager::AllocateTransferQueue(AptnDevice device)
{
	DeviceInternal& deviceInternal = DeviceInternalFrom(device);
	if (CanAllocateQueue(deviceInternal, deviceInternal.graphicsFamilyIndex))
	{
		const u32 handleIndex = PopFreeHandleIndex(deviceInternal.queueHandlePool);
		if (handleIndex != InvalidHandleIndex)
		{
			constexpr u32 queueIndex = 0;
			VkQueue queue = VK_NULL_HANDLE;
			vkGetDeviceQueue(deviceInternal.device, deviceInternal.transferFamilyIndex, queueIndex, &queue);

			QueueInternal queueInternal{
				.queue = queue,
				.queueFamilyIndex = deviceInternal.transferFamilyIndex
			};
			GetQueueInternalFromIndex(deviceInternal, handleIndex) = queueInternal;

			const u32 indexGeneration = GetHandleGeneration(deviceInternal.queueHandlePool, handleIndex);
			// TODO(nblane): this MUST be moved so that it can be reused
			const u64 handleData = (device.handle << DEVICE_INDEX_SHIFT)
				| (((u64)deviceInternal.transferFamilyIndex) << POOL_INDEX_SHIFT)
				| (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
				| (handleIndex & RESOURCE_INDEX_MASK);
			return AptnQueue{ handleData };
		}
	}

	// Here we need to log an error, since all queues are taken at this point
	return { AptnInvalidHandle };
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
//			.queueFamilyIndex = deviceInternal.computeFamilyIndex
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

void DeviceManager::FreeQueue(AptnQueue queue)
{
	// TODO: Assert handle is valid AND handle generation is correct
	DeviceInternal& deviceInternal = GetDeviceInternal(queue);
	PushFreedHandleIndex(deviceInternal.queueHandlePool, GetHandleIndex(queue));
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
	const AptnDebugMessageTypeFlags messageTypeFlags = messageType;
	return userValidation.delegate(severity, messageTypeFlags, pCallbackData, pUserData);
}

AptnCommandPool DeviceManager::CreateCommandPool(AptnDevice deviceHandle, const AptnCommandPoolCreationParams& params)
{
	DeviceInternal& deviceInternal = DeviceInternalFrom(deviceHandle);

	VkCommandPoolCreateInfo createInfo;
	Vk::ZeroInfoStruct(createInfo, VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO);
	createInfo.queueFamilyIndex = params.queueIndex;
	createInfo.flags = params.canResetCommandBuffers ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0;
	VkCommandPool cmdPool;
	VkResult result = vkCreateCommandPool(deviceInternal.device, &createInfo, nullptr, &cmdPool);
	CHECK_VK(result);
	if (result == VK_SUCCESS)
	{
		CommandPoolInternal commandPoolInternal{
			.cmdPool = cmdPool,
			.queueFamilyIndex = params.queueIndex
		};
		const u32 handleIndex = PopFreeHandleIndex(deviceInternal.commandPoolsHandlePool);
		if (handleIndex != InvalidHandleIndex)
		{
			// Set the internal index to be the current data
			// handleIndex is valid, so we want to point to the "previous" element
			// TODO - this kinda is stinky to me, don't assign to the return of a function...
			GetCommandPoolInternalFromIndex(deviceInternal, handleIndex) = commandPoolInternal;

			const u32 indexGeneration = GetHandleGeneration(deviceInternal.commandPoolsHandlePool, handleIndex);
			// TODO(nblane): this MUST be moved so that it can be reused
			const u64 handleData = (deviceHandle.handle << DEVICE_INDEX_SHIFT) 
				| (((u64)indexGeneration) << RESOURCE_GEN_SHIFT) 
				| (handleIndex & RESOURCE_INDEX_MASK);
			return AptnCommandPool{handleData};
		}
	}
	return { AptnInvalidHandle };
}

void DeviceManager::DestroyCommandPool(AptnCommandPool commandPoolHandle)
{
	// TODO: Assert handle is valid AND handle generation is correct
	DeviceInternal& deviceInternal = GetDeviceInternal(commandPoolHandle);
	u32 handleIndex = GetHandleIndex(commandPoolHandle);
	VkCommandPool cmdPool = deviceInternal.commandPools[handleIndex-1].cmdPool;
	vkDestroyCommandPool(deviceInternal.device, cmdPool, nullptr);

	// Let the handle pool know this handle is freed
	PushFreedHandleIndex(deviceInternal.commandPoolsHandlePool, handleIndex);
}

AptnCommandBuffer DeviceManager::AllocateCommandBuffer(AptnCommandPool commandPoolHandle, const AptnCommandBufferAllocParams& params)
{
	DeviceInternal& deviceInternal = GetDeviceInternal(commandPoolHandle);
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
		CommandBufferInternal commandBufferInternal
		{
			.commandBuffer = cmdBuffer,
			.queueFamilyIndex = commandPoolInternal.queueFamilyIndex
		};
		const u32 handleIndex = PopFreeHandleIndex(deviceInternal.commandBufferHandlePool);
		if (handleIndex != InvalidHandleIndex)
		{
			// Set the internal index to be the current data
			// TODO - this kinda is stinky to me, don't assign to the return of a function...
			GetCommandBufferInternalFromIndex(deviceInternal, handleIndex) = commandBufferInternal;

			const u32 indexGeneration = GetHandleGeneration(deviceInternal.commandBufferHandlePool, handleIndex);
			// TODO(nblane): this MUST be moved so that it can be reused
			const u64 handleData = ((u64)GetDeviceIndexFromHandle(commandPoolHandle) << DEVICE_INDEX_SHIFT)
				| ((u64)cmdPoolIndex << POOL_INDEX_SHIFT)
				| (((u64)indexGeneration) << RESOURCE_GEN_SHIFT)
				| (handleIndex & RESOURCE_INDEX_MASK);
			return AptnCommandBuffer{ handleData };
		}
	}
	return { AptnInvalidHandle };
}

void DeviceManager::FreeCommandBuffer(AptnCommandBuffer commandBufferHandle)
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

void DeviceManager::ResetCommandBuffer(AptnCommandBuffer commandBuffer)
{
	u32 deviceIndex = GetDeviceIndexFromHandle(commandBuffer);
	DeviceInternal& deviceInternal = deviceInternals[deviceIndex - 1];
	u32 cmdBufferIndex = GetHandleIndex(commandBuffer);
	CommandBufferInternal& commandBufferInternal = GetCommandBufferInternalFromIndex(deviceInternal, cmdBufferIndex);
	VkResult result = vkResetCommandBuffer(commandBufferInternal.commandBuffer, 0);
	CHECK_VK(result);
}
