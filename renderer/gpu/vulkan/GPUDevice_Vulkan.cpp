#include "GPUDevice_Vulkan.h"
#include "Config_Vulkan.h"
#include "Expected.h"
#include "Logger.h"
#include "CSEAssert.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace CSERenderer
{

vkb::Result<vkb::Instance> CreateInstance();
vkb::Result<vkb::PhysicalDevice> CreatePhysicalDevice(const vkb::Instance& instance);
vkb::Result<vkb::Device> CreateDevice(const vkb::PhysicalDevice& physDevice);
CSECore::Expected<VmaAllocator, std::string> CreateVMAAllocator(const vkb::Instance& instance, const vkb::PhysicalDevice& physDevice, const vkb::Device& device);

#if RENDER_DEBUG_ENABLED
VKAPI_ATTR VkBool32 VKAPI_CALL _debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData);
#endif

GPUDevice_Vulkan::GPUDevice_Vulkan()
	: _instance(),
	_physDevice(),
	_device(),
	_vmaAllocator()
{

}

GPUDevice_Vulkan::~GPUDevice_Vulkan()
{

}

void GPUDevice_Vulkan::Initialize()
{
	CSE_ASSERT(volkInitialize() == VK_SUCCESS, "Failed to initialize volk.");

	vkb::Result<vkb::Instance> instResult = CreateInstance();
	CSE_ASSERT(instResult.has_value(), "Failed to create Vulkan instance. Reason: " << instResult.error().message());
	_instance = instResult.value();
	volkLoadInstance(_instance.instance);

	vkb::Result<vkb::PhysicalDevice> physDeviceResult = CreatePhysicalDevice(_instance);
	CSE_ASSERT(physDeviceResult.has_value(), "Failed to create Vulkan physical device. Reason: " << physDeviceResult.error().message());
	_physDevice = physDeviceResult.value();

	vkb::Result<vkb::Device> deviceResult = CreateDevice(_physDevice);
	CSE_ASSERT(deviceResult.has_value(), "Failed to create Vulkan device. Reason: " << deviceResult.error().message());
	_device = deviceResult.value();
	volkLoadDevice(_device.device);

	CSECore::Expected<VmaAllocator, std::string> vmaResult = CreateVMAAllocator(_instance, _physDevice, _device);
	CSE_ASSERT(vmaResult.HasExpected(), "Failed to create VMA allocator. Reason: " << vmaResult.GetUnexpected());
	_vmaAllocator = vmaResult.GetExpected();
}

void GPUDevice_Vulkan::Dispose()
{
	vmaDestroyAllocator(_vmaAllocator);
	vkb::destroy_device(_device);
	vkb::destroy_instance(_instance);
}

const VkInstance GPUDevice_Vulkan::GetVkInstance()
{
	return _instance.instance;
}

const VkPhysicalDevice GPUDevice_Vulkan::GetVkPhysicalDevice()
{
	return _physDevice.physical_device;
}

const VkDevice GPUDevice_Vulkan::GetVkDevice()
{
	return _device.device;
}

const VmaAllocator GPUDevice_Vulkan::GetVMAAllocator()
{
	return _vmaAllocator;
}

uint32_t GPUDevice_Vulkan::GetGraphicsQueueFamilyIndex()
{
	return _device.get_queue_index(vkb::QueueType::graphics).value();
}

uint32_t GPUDevice_Vulkan::GetTransferQueueFamilyIndex()
{
	return _device.get_queue_index(vkb::QueueType::transfer).value();
}

uint32_t GPUDevice_Vulkan::GetPresentQueueFamilyIndex()
{
	return _device.get_queue_index(vkb::QueueType::present).value();
}

vkb::Result<vkb::Instance> CreateInstance()
{
	vkb::InstanceBuilder instanceBuilder{};
	instanceBuilder.set_app_name("CSERenderer");
	instanceBuilder.set_app_version(0, 0, 1);
	instanceBuilder.set_engine_version(0, 0, 1);
	instanceBuilder.require_api_version(VULKAN_VERSION_MAJOR, VULKAN_VERSION_MINOR, VULKAN_VERSION_PATCH);

#if RENDER_DEBUG_ENABLED

	vkb::Result<vkb::SystemInfo> sysInfoResult = vkb::SystemInfo::get_system_info();
	
	if (sysInfoResult.has_value() && sysInfoResult->validation_layers_available)
	{
		instanceBuilder.request_validation_layers();
		instanceBuilder.set_debug_messenger_severity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);
		instanceBuilder.add_debug_messenger_type(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);
		instanceBuilder.set_debug_callback(_debugCallback);
	}
#endif

	return instanceBuilder.build();
}

vkb::Result<vkb::PhysicalDevice> CreatePhysicalDevice(const vkb::Instance& instance)
{
	vkb::PhysicalDeviceSelector physDeviceSelector(instance);

	int numExtensions = sizeof(REQUIRED_PHYSICAL_EXTENSIONS) / sizeof(const char*);
	physDeviceSelector.add_required_extensions(std::vector<const char*>(REQUIRED_PHYSICAL_EXTENSIONS, REQUIRED_PHYSICAL_EXTENSIONS + numExtensions));
	physDeviceSelector.set_required_features(GetRequiredVulkanFeatures());
	physDeviceSelector.set_required_features_11(GetRequiredVulkan11Features());
	physDeviceSelector.set_required_features_12(GetRequiredVulkan12Features());
	physDeviceSelector.set_required_features_13(GetRequiredVulkan13Features());
	physDeviceSelector.require_present();
	physDeviceSelector.defer_surface_initialization();

	return physDeviceSelector.select();
}

vkb::Result<vkb::Device> CreateDevice(const vkb::PhysicalDevice& physDevice)
{
	vkb::DeviceBuilder deviceBuilder(physDevice);
	return deviceBuilder.build();
}

CSECore::Expected<VmaAllocator, std::string> CreateVMAAllocator(const vkb::Instance& instance, const vkb::PhysicalDevice& physDevice, const vkb::Device& device)
{
	VmaAllocatorCreateInfo vmaAllocCreateInfo{};
	vmaAllocCreateInfo.instance = instance.instance;
	vmaAllocCreateInfo.physicalDevice = physDevice.physical_device;
	vmaAllocCreateInfo.device = device.device;
	vmaAllocCreateInfo.pHeapSizeLimit = nullptr;
	vmaAllocCreateInfo.pTypeExternalMemoryHandleTypes = nullptr;

	VmaVulkanFunctions vkFuncs;
	vmaImportVulkanFunctionsFromVolk(&vmaAllocCreateInfo, &vkFuncs);

	vmaAllocCreateInfo.pVulkanFunctions = &vkFuncs;

	VmaAllocator vmaAlloc;
	VkResult result = vmaCreateAllocator(&vmaAllocCreateInfo, &vmaAlloc);

	if (result != VK_SUCCESS)
	{
		switch (result)
		{
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			return CSECore::CreateUnexpected<VmaAllocator, std::string>(std::string("Out of host memory."));
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return CSECore::CreateUnexpected<VmaAllocator, std::string>(std::string("Out of device memory."));
		case VK_ERROR_INITIALIZATION_FAILED:
			return CSECore::CreateUnexpected<VmaAllocator, std::string>(std::string("Initialization failed."));
		default:
			return CSECore::CreateUnexpected<VmaAllocator, std::string>(std::string("Unknown error."));
		}
	}

	return CSECore::CreateExpected<VmaAllocator, std::string>(vmaAlloc);
}

#if RENDER_DEBUG_ENABLED
VKAPI_ATTR VkBool32 VKAPI_CALL _debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
{
	switch(messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		CSE_LOGI(callbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		CSE_LOGW(callbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		CSE_LOGE(callbackData->pMessage);
		break;
	default:
		CSE_LOGI(callbackData->pMessage);
	}

	return VK_FALSE;
}
#endif

}