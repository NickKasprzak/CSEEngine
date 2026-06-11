#include "GPUBackend_Vulkan.h"
#include "GPUBuffer_Vulkan.h"
#include "GPUImage_Vulkan.h"
#include "GPUPipeline_Vulkan.h"
#include "internal/GPUPipelineBuilder_Vulkan.h"
#include "internal/Vertex_Vulkan.h"
#include "internal/ShaderProcessor_Vulkan.h"
#include "internal/Utils_Vulkan.h"
#include "../../utility/PlatformWindowInfo_Renderer.h"
#include "Expected.h"
#include "Assert.h"
#include "Logger.h"

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
VmaAllocationCreateFlags ImageUsageToVmaFlags(ImageUsageFlags usage);
VkImageViewType ImageArrayLayersToViewType(uint32_t arrayLayers);
VkImageAspectFlags ImageUsageToImageAspectFlags(VkImageUsageFlags usageFlags);
VkFilter FilterToVkFilter(SamplerState::SamplerFilterMode filterMode);
VkSamplerAddressMode AddressModeToVkAddressMode(SamplerState::SamplerAddressMode addressMode);

GPUBackend_Vulkan::GPUBackend_Vulkan()
	: _device(), _windowSurface(), _swapchain(), _currentFrame(0), _renderGraph(), _descriptorSetPool(), _pipelineRegistry(), _dataLayoutRegistry()
{

}

GPUBackend_Vulkan::~GPUBackend_Vulkan()
{

}

GPUBackend_Vulkan* GPUBackend_Vulkan::Instance_Vulkan()
{
	return static_cast<GPUBackend_Vulkan*>(Instance());
}

void GPUBackend_Vulkan::Initialize()
{
	_device.Initialize();
	_descriptorSetPool.Initialize(_device.GetVkDevice());
	_pipelineRegistry.Initialize(_device.GetVkDevice());
}

void GPUBackend_Vulkan::Dispose()
{
	vkb::destroy_swapchain(_swapchain);
	vkDestroySurfaceKHR(_device.GetVkInstance(), _windowSurface, nullptr);

	_pipelineRegistry.Dispose();
	_descriptorSetPool.Dispose();
	_device.Dispose();
}

uint32_t GPUBackend_Vulkan::GetCurrentFrame()
{
	return _currentFrame;
}

CSECore::Ref<GPUBuffer> GPUBackend_Vulkan::CreateBuffer(const BufferCreateInfo& createInfo)
{
	VkBufferUsageFlags usage = BufferUsageToVulkanUsageFlags(createInfo.usage);
	uint32_t queueFamily = _device.GetGraphicsQueueFamilyIndex();

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.size = createInfo.size;
	bufferCreateInfo.queueFamilyIndexCount = 1;
	bufferCreateInfo.pQueueFamilyIndices = &queueFamily;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocCreateInfo.flags = BufferUsageToVmaFlags(createInfo.usage);

	VkBuffer buffer;
	VmaAllocation allocation;
	VkResult result = vmaCreateBuffer(_device.GetVMAAllocator(), &bufferCreateInfo, &allocCreateInfo, &buffer, &allocation, nullptr);
	if (result != VK_SUCCESS)
	{
		switch (result)
		{
		default:
			CSE_LOGE("Failed to create buffer. Reason: Unknown error.");
		}
		return CSECore::Ref<GPUBuffer>();
	}

	return CSECore::Ref<GPUBuffer>(new GPUBuffer_Vulkan(buffer, usage, createInfo.size, _device.GetVMAAllocator(), allocation));
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

CSECore::Ref<GPUImage> GPUBackend_Vulkan::CreateImage(const ImageCreateInfo& createInfo)
{
	VkImageUsageFlags usage = ImageUsageToVulkanUsageFlags(createInfo.usage);
	VkFormat format = ImageFormatToVkFormat(createInfo.format);
	VkImageAspectFlags aspect = ImageUsageToImageAspectFlags(createInfo.usage);

	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.usage = usage;
	imageCreateInfo.format = format;
	imageCreateInfo.extent.width = createInfo.width;
	imageCreateInfo.extent.height = createInfo.height;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocCreateInfo.flags = ImageUsageToVmaFlags(createInfo.usage);

	VkImage image;
	VmaAllocation allocation;
	VkResult imageResult = vmaCreateImage(_device.GetVMAAllocator(), &imageCreateInfo, &allocCreateInfo, &image, &allocation, nullptr);
	if (imageResult != VK_SUCCESS)
	{
		switch (imageResult)
		{
		default:
			CSE_LOGE("Failed to create image. Reason: Unknown error.");
		}
		return CSECore::Ref<GPUImage>();
	}

	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	imageViewCreateInfo.subresourceRange.aspectMask = aspect;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1; 
	imageViewCreateInfo.subresourceRange.baseMipLevel = 1;
	imageViewCreateInfo.subresourceRange.levelCount = 1;

	VkImageView imageView{};
	VkResult imageViewResult = vkCreateImageView(_device.GetVkDevice(), &imageViewCreateInfo, nullptr, &imageView);
	if (imageViewResult != VK_SUCCESS)
	{
		switch (imageViewResult)
		{
		default:
			CSE_LOGE("Failed to create image view. Reason: Unknown error.");
		}
		return CSECore::Ref<GPUImage>();
	}

	return CSECore::Ref<GPUImage>(new GPUImage_Vulkan(_device.GetVkDevice(), image, imageView, usage, VK_IMAGE_VIEW_TYPE_2D, _device.GetVMAAllocator(), allocation));
}

void GPUBackend_Vulkan::ImageCopy(CSECore::Ref<GPUImage> image)
{
	GPUImage_Vulkan* vkImage = static_cast<GPUImage_Vulkan*>(image.GetRawPointer());
	CSE_ASSERT(vkImage, "Couldn't copy to image as the given image handle is null.");

	CSE_ASSERT(false, "Reminder to check for transfer dest and passing off image copy request to compositor.");
}

CSECore::Ref<GPUPipeline> GPUBackend_Vulkan::CreateGraphicsPipeline(const PipelineInfo& pipelineInfo)
{
	CSE_ASSERT(pipelineInfo.shaderCount >= 2, "Creating a graphics pipeline requires at least 2 shaders.");

	// Check if a pipeline with the required state already exists

	CSECore::Ref<GPUPipeline> existingPipeline = _pipelineRegistry.GetGraphicsPipeline(pipelineInfo);
	if (existingPipeline.GetRawPointer() != nullptr)
	{
		return existingPipeline;
	}

	// Process shader layouts

	CSECore::Expected<ShaderLayoutInfo, std::string> shaderProcessResult = ProcessGraphicsShaderLayout(pipelineInfo.shaders[0].shaderCode, pipelineInfo.shaders[1].shaderCode);
	if (shaderProcessResult.HasUnexpected())
	{
		CSE_LOGE("Failed to create graphics pipeline. Reason: " << shaderProcessResult.GetUnexpected());
		return CSECore::Ref<GPUPipeline>();
	}
	ShaderLayoutInfo* shaderLayout = shaderProcessResult.GetExpectedPtr();

	// Validate descriptor set layout, vertex attributes, and SSBO layouts

	if (ValidateShaderDescriptorSetLayout(shaderLayout->descriptorSetLayoutInfo) == false)
	{
		CSE_LOGE("Failed to create graphics pipeline. Reason: Unsupported descriptor set layout.");
		return CSECore::Ref<GPUPipeline>();
	}

	if (ValidateShaderVertexAttributes(shaderLayout->vertexAttributes) == false)
	{
		CSE_LOGE("Failed to create graphics pipeline. Reason: Unsupported vertex layout.");
		return CSECore::Ref<GPUPipeline>();
	}

	for (int i = 0; i < shaderLayout->ssboLayouts.size(); i++)
	{
		CSECore::Ref<GPUDataLayout> layoutRef = _dataLayoutRegistry.GetDataLayout(shaderLayout->ssboLayouts[i].GetNameHash());
		if (layoutRef.GetRawPointer() != nullptr &&
			*layoutRef.GetRawPointer() == shaderLayout->ssboLayouts[i])
		{
			CSE_LOGE("Failed to create graphics pipeline. Reason: An SSBO input layout with a given name already exists with a different layout.");
			return CSECore::Ref<GPUPipeline>();
		}
	}

	// Build the pipeline

	GPUPipelineBuilder_Vulkan builder(_device.GetVkDevice());
	builder.SetGraphicsShaderInfo(pipelineInfo.shaders[0], pipelineInfo.shaders[1]);
	builder.SetLayoutInfo(_descriptorSetPool.GetDescriptorSetLayout(), shaderLayout->pushConstantLayouts);
	builder.SetViewportInfo(*pipelineInfo.viewportInfo);
	builder.SetRasterizationInfo(*pipelineInfo.rasterizationInfo);
	builder.SetMultisampleInfo(*pipelineInfo.multisampleInfo);
	builder.SetDepthStencilInfo(*pipelineInfo.depthStencilInfo);
	builder.SetColorBlendInfo(*pipelineInfo.colorBlendInfo);
	builder.SetAttachmentInfo(*pipelineInfo.attachmentInfo);
	builder.SetDynamicStateInfo(*pipelineInfo.dynamicStateInfo);

	CSECore::Expected<GPUPipelineBuilderResult_Vulkan, std::string> buildResult = builder.Build();
	if (buildResult.HasUnexpected())
	{
		CSE_LOGE("Failed to create graphics pipeline. Reason: " << buildResult.GetUnexpected());
		return CSECore::Ref<GPUPipeline>();
	}
	GPUPipelineBuilderResult_Vulkan* pipelineHandles = buildResult.GetExpectedPtr();

	// Register SSBO layouts in the layout registry

	std::vector<CSECore::Ref<GPUDataLayout>> ssboLayoutRefs;
	for (int i = 0; i < shaderLayout->ssboLayouts.size(); i++)
	{
		ssboLayoutRefs.push_back(_dataLayoutRegistry.AddDataLayout(shaderLayout->ssboLayouts[i]));
	}

	// Create a RenderAttachmentLayout from the given pipeline info

	std::vector<VkFormat> colorAttachments;
	for (int i = 0; i < pipelineInfo.attachmentInfo->colorAttachmentCount; i++)
	{
		colorAttachments.push_back(ImageFormatToVkFormat(pipelineInfo.attachmentInfo->colorAttachmentFormats[i]));
	}
	VkFormat depthFormat = ImageFormatToVkFormat(pipelineInfo.attachmentInfo->depthAttachmentFormat);
	VkFormat stencilFormat = ImageFormatToVkFormat(pipelineInfo.attachmentInfo->stencilAttachmentFormat);

	RenderAttachmentLayout attachments(colorAttachments, depthFormat, stencilFormat);

	// Create and register the GPUPipeline object

	GPUPipelineParams_Vulkan pipelineParams{};
	pipelineParams.device = _device.GetVkDevice();
	pipelineParams.pipeline = pipelineHandles->pipeline;
	pipelineParams.layout = pipelineHandles->layout;
	pipelineParams.ssboLayouts = &ssboLayoutRefs;
	pipelineParams.pushConstantLayouts = &shaderLayout->pushConstantLayouts;
	pipelineParams.renderAttachmentLayout = &attachments;
	pipelineParams.pipelineInfo = &pipelineInfo;

	CSECore::Ref<GPUPipeline> pipelineRef = _pipelineRegistry.RegisterGraphicsPipeline(pipelineParams);

	return pipelineRef;
}

CSECore::Ref<SSBO_Vulkan> GPUBackend_Vulkan::CreateSSBO(size_t size)
{
	return _ssboAllocator.CreateSSBO(size);
}

CSECore::Ref<SSBODescriptor> GPUBackend_Vulkan::CreateSSBODescriptor(CSECore::Ref<SSBO_Vulkan> ssbo)
{
	return _descriptorSetPool.CreateSSBODescriptor(ssbo);
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

VmaAllocationCreateFlags ImageUsageToVmaFlags(ImageUsageFlags usage)
{
	VmaAllocationCreateFlags flags = 0;

	if (usage & IMAGE_USAGE_MAPPED)
	{
		flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
	}

	return flags;
}

VkImageViewType ImageArrayLayersToViewType(uint32_t arrayLayers)
{
	CSE_ASSERT(arrayLayers != 0, "Can't use an array layer count of 0.");

	if (arrayLayers == 1)
	{
		return VK_IMAGE_VIEW_TYPE_2D;
	}
	else if (arrayLayers == 6)
	{
		return VK_IMAGE_VIEW_TYPE_CUBE;
	}
	else if (arrayLayers % 6 == 0)
	{
		return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
	}
	return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
}

VkImageAspectFlags ImageUsageToImageAspectFlags(VkImageUsageFlags usageFlags)
{
	VkImageAspectFlags flags = 0;

	if (usageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
	{
		flags |= VK_IMAGE_ASPECT_COLOR_BIT;
	}
	else if (usageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
	{
		flags |= (VK_IMAGE_ASPECT_DEPTH_BIT & VK_IMAGE_ASPECT_STENCIL_BIT);
	}

	return flags;
}

VkFilter FilterToVkFilter(SamplerState::SamplerFilterMode filterMode)
{
	switch (filterMode)
	{
	case SamplerState::SamplerFilterMode::FILTER_NEAREST:
		return VK_FILTER_NEAREST;
	case SamplerState::SamplerFilterMode::FILTER_LINEAR:
		return VK_FILTER_LINEAR;
	default:
		return VK_FILTER_MAX_ENUM;
	}
}

VkSamplerAddressMode AddressModeToVkAddressMode(SamplerState::SamplerAddressMode addressMode)
{
	switch (addressMode)
	{
	case SamplerState::SamplerAddressMode::ADDRESS_MODE_CLAMP_TO_BORDER:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
	case SamplerState::SamplerAddressMode::ADDRESS_MODE_CLAMP_TO_EDGE:
		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	case SamplerState::SamplerAddressMode::ADDRESS_MODE_REPEAT:
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	default:
		return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
	}
}

}