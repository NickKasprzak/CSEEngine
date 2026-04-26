#include "GPUImage_Vulkan.h"
#include "Logger.h"
#include "CSEAssert.h"

namespace CSERenderer
{

VkImageViewType ImageArrayLayersToViewType(uint32_t arrayLayers);
VkImageAspectFlags ImageUsageToImageAspectFlags(VkImageUsageFlags usageFlags);

GPUImage_Vulkan::GPUImage_Vulkan()
	: _image(VK_NULL_HANDLE), _imageView(VK_NULL_HANDLE), _info(), _allocator(VK_NULL_HANDLE), _allocation(VK_NULL_HANDLE), _sampler(VK_NULL_HANDLE)
{
	
}

GPUImage_Vulkan::GPUImage_Vulkan(VulkanImageInfo* params, uint32_t queueFamily, VmaAllocator allocator)
	: _image(VK_NULL_HANDLE), _imageView(VK_NULL_HANDLE), _info(), _allocator(VK_NULL_HANDLE), _allocation(VK_NULL_HANDLE), _sampler(VK_NULL_HANDLE)
{
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.usage = params->usage;
	imageCreateInfo.format = params->format;
	imageCreateInfo.extent = params->extent;
	imageCreateInfo.mipLevels = params->mipLevels;
	imageCreateInfo.arrayLayers = params->arrayLayers;
	imageCreateInfo.samples = params->samples;
	imageCreateInfo.flags = params->createFlags;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocCreateInfo.flags = params->alloc;

	VkImage image;
	VmaAllocation allocation;
	VkResult imageResult = vmaCreateImage(allocator, &imageCreateInfo, &allocCreateInfo, &image, &allocation, nullptr);
	if (imageResult != VK_SUCCESS)
	{
		switch (imageResult)
		{
		default:
			CSE_LOGE("Failed to create image. Reason: Unknown error.");
			return;
		}
	}

	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = ImageArrayLayersToViewType(params->arrayLayers);
	imageViewCreateInfo.format = params->format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	imageViewCreateInfo.subresourceRange.aspectMask = ImageUsageToImageAspectFlags(params->usage);
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = params->arrayLayers;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 1;
	imageViewCreateInfo.subresourceRange.levelCount = params->mipLevels;

	VkImageView imageView{};
	VkResult imageViewResult = vkCreateImageView(params->device, & imageViewCreateInfo, nullptr, & _imageView);
	if (imageViewResult != VK_SUCCESS)
	{
		switch (imageViewResult)
		{
		default:
			CSE_LOGE("Failed to create image view. Reason: Unknown error.");
			return;
		}
	}

	_image = image;
	_imageView = imageView;
	_info = *params;
	_allocator = allocator;
	_allocation = allocation;
}

GPUImage_Vulkan::~GPUImage_Vulkan()
{
	if (_image != VK_NULL_HANDLE)
	{
		if (_sampler != VK_NULL_HANDLE)
		{
			vkDestroySampler(_info.device, _sampler, nullptr);
		}

		vkDestroyImageView(_info.device, _imageView, nullptr);
		vmaDestroyImage(_allocator, _image, _allocation);
	}
}

VkImage GPUImage_Vulkan::GetImageHandle()
{
	return _image;
}

VkImageView GPUImage_Vulkan::GetImageViewHandle()
{
	return _imageView;
}

VkImageUsageFlags GPUImage_Vulkan::GetImageUsage()
{
	return _info.usage;
}

VkImageViewType GPUImage_Vulkan::GetImageViewType()
{
	return ImageArrayLayersToViewType(_info.arrayLayers);
}

void GPUImage_Vulkan::SetSampler(VkFilter filter, VkSamplerAddressMode addressMode)
{
	CSE_ASSERT(_info.usage & VK_IMAGE_USAGE_SAMPLED_BIT, "Image having sampler set doesn't have correct usage flags.");

	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.minFilter = filter;
	samplerCreateInfo.magFilter = filter;
	samplerCreateInfo.addressModeU = addressMode;
	samplerCreateInfo.addressModeV = addressMode;
	samplerCreateInfo.addressModeW = addressMode;
	samplerCreateInfo.anisotropyEnable = VK_FALSE;
	samplerCreateInfo.maxAnisotropy = 1;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_EQUAL;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 1.0f;

	VkSampler sampler;
	VkResult result = vkCreateSampler(_info.device, &samplerCreateInfo, nullptr, &sampler);
	if (result != VK_SUCCESS)
	{
		switch (result)
		{
			CSE_LOGE("Failed to create image sampler. Reason: Unknown error.");
		}
	}

	_sampler = sampler;
}

VkSampler GPUImage_Vulkan::GetSampler()
{
	return _sampler;
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

}