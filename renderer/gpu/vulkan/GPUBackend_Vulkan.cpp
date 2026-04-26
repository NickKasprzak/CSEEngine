#include "GPUBackend_Vulkan.h"
#include "GPUBuffer_Vulkan.h"
#include "GPUImage_Vulkan.h"
#include "../../utility/PlatformWindowInfo_Renderer.h"
#include "Expected.h"
#include "Assert.h"

namespace CSERenderer
{

struct WindowSurfaceInfo
{
	VkSurfaceKHR surface;
	uint16_t width;
	uint16_t height;
};

CSECore::Expected<WindowSurfaceInfo, std::string> CreateWindowSurface(const VkInstance instance, const CSECore::Any<64>& windowInfo);
vkb::Result<vkb::Swapchain> CreateSwapchain(const VkPhysicalDevice physicalDevice, const VkDevice device, const WindowSurfaceInfo surfaceInfo);
vkb::Result<vkb::Swapchain> ResizeSwapchain(const VkPhysicalDevice physicalDevice, const VkDevice device, const vkb::Swapchain& oldSwapchain, const WindowSurfaceInfo newSurfaceInfo);

VkBufferUsageFlags BufferUsageToVulkanUsageFlags(BufferUsageFlags usage);
VmaAllocationCreateFlags BufferUsageToVmaFlags(BufferUsageFlags usage);

VkImageUsageFlags ImageUsageToVulkanUsageFlags(ImageUsageFlags usage);
VkFormat ImageUsageToVulkanFormat(ImageUsageFlags usage);
VmaAllocationCreateFlags ImageUsageToVmaFlags(ImageUsageFlags usage);
VkFilter FilterToVkFilter(SamplerFilterMode filterMode);
VkSamplerAddressMode AddressModeToVkAddressMode(SamplerAddressMode addressMode);

GPUBackend_Vulkan::GPUBackend_Vulkan()
	: _device(), _windowSurface(), _swapchain(), _compositor()
{

}

GPUBackend_Vulkan::~GPUBackend_Vulkan()
{

}

void GPUBackend_Vulkan::Initialize()
{
	_device.Initialize();
}

void GPUBackend_Vulkan::Dispose()
{
	vkb::destroy_swapchain(_swapchain);
	vkDestroySurfaceKHR(_device.GetVkInstance(), _windowSurface, nullptr);
	_device.Dispose();
}

CSECore::Ref<GPUBuffer> GPUBackend_Vulkan::CreateBuffer(BufferUsageFlags usage, uint32_t size)
{
	VulkanBufferInfo bufferParams{};
	bufferParams.usage = BufferUsageToVulkanUsageFlags(usage);
	bufferParams.alloc = BufferUsageToVmaFlags(usage);
	bufferParams.size = size;
	uint32_t queueFamily = _device.GetGraphicsQueueFamilyIndex();

	/*
	* Replace with pooled allocation of GPUBuffer_Vulkan
	* later. This will require Ref to
	* support a deleter callback function.
	*/
	GPUBuffer_Vulkan* buffer = new GPUBuffer_Vulkan(&bufferParams, queueFamily, _device.GetVMAAllocator());
	if (buffer->GetHandle() == nullptr)
	{
		return CSECore::Ref<GPUBuffer>(nullptr);
	}

	if (usage & BUFFER_USAGE_MAPPED)
	{
		buffer->MapBuffer();
	}

	return CSECore::Ref<GPUBuffer>(buffer);
}

void GPUBackend_Vulkan::BufferWrite(CSECore::Ref<GPUBuffer> buffer)
{
	GPUBuffer_Vulkan* vkBuffer = static_cast<GPUBuffer_Vulkan*>(buffer.GetRawPointer());
	CSE_ASSERT(vkBuffer, "Couldnt't write to buffer as the given buffer handle is null.");

	CSE_ASSERT(false, "Reminder to check for transfer source before memcpy.");
}

void GPUBackend_Vulkan::BufferCopy(CSECore::Ref<GPUBuffer> buffer)
{
	GPUBuffer_Vulkan* vkBuffer = static_cast<GPUBuffer_Vulkan*>(buffer.GetRawPointer());
	CSE_ASSERT(vkBuffer, "Couldnt't copy to buffer as the given buffer handle is null.");

	CSE_ASSERT(false, "Reminder to check for transfer dest and passing off buffer copy request to compositor.");
}

CSECore::Ref<GPUImage> GPUBackend_Vulkan::CreateImage(ImageUsageFlags usage, uint32_t width, uint32_t height)
{
	VulkanImageInfo imageParams{};
	imageParams.usage = ImageUsageToVulkanUsageFlags(usage);
	imageParams.format = ImageUsageToVulkanFormat(usage);
	imageParams.extent.width = width;
	imageParams.extent.height = height;
	imageParams.extent.depth = 1;
	imageParams.mipLevels = 1;
	imageParams.arrayLayers = 1;
	imageParams.samples = VK_SAMPLE_COUNT_1_BIT;
	imageParams.alloc = ImageUsageToVmaFlags(usage);
	imageParams.device = _device.GetVkDevice();
	uint32_t queueFamily = _device.GetGraphicsQueueFamilyIndex();

	/*
	* Replace with pooled allocation of GPUImage_Vulkan
	* later. This will require Ref to
	* support a deleter callback function.
	*/
	return CSECore::Ref<GPUImage>(new GPUImage_Vulkan(&imageParams, queueFamily, _device.GetVMAAllocator()));
}

void GPUBackend_Vulkan::SetImageSampler(CSECore::Ref<GPUImage> image, SamplerFilterMode filter, SamplerAddressMode addressMode)
{
	GPUImage_Vulkan* vkImage = static_cast<GPUImage_Vulkan*>(image.GetRawPointer());
	CSE_ASSERT(vkImage, "Couldn't set image sampler as the given image handle is null.");

	VkFilter vkFilter = FilterToVkFilter(filter);
	VkSamplerAddressMode vkAddressMode = AddressModeToVkAddressMode(addressMode);
	vkImage->SetSampler(vkFilter, vkAddressMode);
}

void GPUBackend_Vulkan::ImageCopy(CSECore::Ref<GPUImage> image)
{
	GPUImage_Vulkan* vkImage = static_cast<GPUImage_Vulkan*>(image.GetRawPointer());
	CSE_ASSERT(vkImage, "Couldn't copy to image as the given image handle is null.");

	CSE_ASSERT(false, "Reminder to check for transfer dest and passing off image copy request to compositor.");
}

void GPUBackend_Vulkan::SetTargetWindow(const CSECore::Any<64>& windowInfo)
{
	CSECore::Expected<WindowSurfaceInfo, std::string> surfaceResult = CreateWindowSurface(_device.GetVkInstance(), windowInfo);
	CSE_ASSERT(surfaceResult.HasExpected(), "Failed to create surface for given window. Reason: " << surfaceResult.GetUnexpected());
	WindowSurfaceInfo surfaceInfo = surfaceResult.GetExpected();
	_windowSurface = surfaceInfo.surface;

	vkb::Result<vkb::Swapchain> swapchainResult = CreateSwapchain(_device.GetVkPhysicalDevice(), _device.GetVkDevice(), surfaceInfo);
	CSE_ASSERT(swapchainResult.has_value(), "Failed to create swapchain. Reason: " << swapchainResult.error().message());
	_swapchain = swapchainResult.value();
}

void GPUBackend_Vulkan::UpdateWindowSurfaceSize(uint16_t width, uint16_t height)
{
	WindowSurfaceInfo newSurfaceInfo;
	newSurfaceInfo.surface = _windowSurface;
	newSurfaceInfo.width = width;
	newSurfaceInfo.height = height;

	vkb::Result<vkb::Swapchain> swapchainResult = ResizeSwapchain(_device.GetVkPhysicalDevice(), _device.GetVkDevice(), _swapchain, newSurfaceInfo);
	CSE_ASSERT(swapchainResult.has_value(), "Failed to resize swapchain. Reason: " << swapchainResult.error().message());
	_swapchain = swapchainResult.value();
}

vkb::Result<vkb::Swapchain> CreateSwapchain(const VkPhysicalDevice physicalDevice, const VkDevice device, const WindowSurfaceInfo surfaceInfo)
{
	vkb::SwapchainBuilder swapchainBuilder(physicalDevice, device, surfaceInfo.surface);
	swapchainBuilder.use_default_format_selection();
	swapchainBuilder.use_default_image_usage_flags();
	swapchainBuilder.use_default_present_mode_selection();
	swapchainBuilder.set_desired_extent(surfaceInfo.width, surfaceInfo.height);

	return swapchainBuilder.build();
}

vkb::Result<vkb::Swapchain> ResizeSwapchain(const VkPhysicalDevice physicalDevice, const VkDevice device, const vkb::Swapchain& oldSwapchain, const WindowSurfaceInfo newSurfaceInfo)
{
	vkb::SwapchainBuilder swapchainBuilder(physicalDevice, device, newSurfaceInfo.surface);
	swapchainBuilder.use_default_format_selection();
	swapchainBuilder.use_default_image_usage_flags();
	swapchainBuilder.use_default_present_mode_selection();
	swapchainBuilder.set_desired_extent(newSurfaceInfo.width, newSurfaceInfo.height);
	swapchainBuilder.set_old_swapchain(oldSwapchain);

	return swapchainBuilder.build();
}

#if WIN32

CSECore::Expected<WindowSurfaceInfo, std::string> CreateWindowSurface(const VkInstance instance, const CSECore::Any<64>& windowInfo)
{
	if (windowInfo.IsA<PlatformWindowInfo_Win32>() == false)
	{
		return CSECore::CreateUnexpected<WindowSurfaceInfo, std::string>(std::string("Given window info doesn't contain a PlatformWindowInfo_Win32."));
	}

	PlatformWindowInfo_Win32 infoWin32 = windowInfo.CastTo<PlatformWindowInfo_Win32>();

	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = infoWin32.hinst;
	surfaceCreateInfo.hwnd = infoWin32.hwnd;

	VkSurfaceKHR surface;
	VkResult surfaceCreateResult = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
	if (surfaceCreateResult != VK_SUCCESS)
	{
		switch (surfaceCreateResult)
		{
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return CSECore::CreateUnexpected<WindowSurfaceInfo, std::string>(std::string("Out of device memory."));
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			return CSECore::CreateUnexpected<WindowSurfaceInfo, std::string>(std::string("Out of host memory."));
		case VK_ERROR_VALIDATION_FAILED_EXT:
			return CSECore::CreateUnexpected<WindowSurfaceInfo, std::string>(std::string("Validation failed."));
		default:
			return CSECore::CreateUnexpected<WindowSurfaceInfo, std::string>(std::string("Unknown error."));
		}
	}

	WindowSurfaceInfo winInfo{};
	winInfo.surface = surface;
	winInfo.width = infoWin32.width;
	winInfo.height = infoWin32.height;

	return CSECore::CreateExpected<WindowSurfaceInfo, std::string>(winInfo);
}

#endif

VkBufferUsageFlags BufferUsageToVulkanUsageFlags(BufferUsageFlags usage)
{
	VkBufferUsageFlags flags = 0;

	if (usage & BUFFER_USAGE_VERTEX)
	{
		flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	}
	if (usage & BUFFER_USAGE_INDEX)
	{
		flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	}
	if (usage & BUFFER_USAGE_STORAGE)
	{
		flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	}
	if (usage & BUFFER_USAGE_UNIFORM)
	{
		flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	}

	if (usage & BUFFER_USAGE_MAPPED)
	{
		flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	}
	else if (~usage & BUFFER_USAGE_MAPPED)
	{
		flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}

	return flags;
}

VmaAllocationCreateFlags BufferUsageToVmaFlags(BufferUsageFlags usage)
{
	VmaAllocationCreateFlags flags = 0;

	if (usage & BUFFER_USAGE_MAPPED)
	{
		flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	}

	return flags;
}

VkImageUsageFlags ImageUsageToVulkanUsageFlags(ImageUsageFlags usage)
{
	VkImageUsageFlags flags = 0;

	if (usage & IMAGE_USAGE_SAMPLED)
	{
		flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
	}

	if (usage & IMAGE_USAGE_COLOR_ATTACHMENT)
	{
		flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	else if (usage & IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT)
	{
		flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	}

	if (usage & IMAGE_USAGE_STORAGE)
	{
		flags |= VK_IMAGE_USAGE_STORAGE_BIT;
	}

	if (usage & IMAGE_USAGE_MAPPED)
	{
		flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}
	else if (~usage & IMAGE_USAGE_MAPPED)
	{
		flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}

	return flags;
}

VkFormat ImageUsageToVulkanFormat(ImageUsageFlags usage)
{
	VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

	if (usage & IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT)
	{
		format = VK_FORMAT_D32_SFLOAT_S8_UINT;
	}
	
	return format;
}

VmaAllocationCreateFlags ImageUsageToVmaFlags(ImageUsageFlags usage)
{
	VmaAllocationCreateFlags flags = 0;

	if (usage & IMAGE_USAGE_MAPPED)
	{
		flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	}

	return flags;
}

VkFilter FilterToVkFilter(SamplerFilterMode filterMode)
{
	switch (filterMode)
	{
	case FILTER_NEAREST:
		return VK_FILTER_NEAREST;
	case FILTER_LINEAR:
		return VK_FILTER_LINEAR;
	default:
		return VK_FILTER_MAX_ENUM;
	}
}

VkSamplerAddressMode AddressModeToVkAddressMode(SamplerAddressMode addressMode)
{
	switch (addressMode)
	{
	case ADDRESS_MODE_CLAMP_TO_BORDER:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	case ADDRESS_MODE_CLAMP_TO_EDGE:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case ADDRESS_MODE_REPEAT:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	default:
		return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
	}
}

}