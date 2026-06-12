#include "GPUImage_Vulkan.h"
#include "Logger.h"
#include "CSEAssert.h"

namespace CSERenderer
{

uint32_t MakeImageID();

GPUImage::GPUImage()
	: _device(VK_NULL_HANDLE),
	_image(VK_NULL_HANDLE), 
	_imageView(VK_NULL_HANDLE), 
	_usage(0), 
	_imageViewType(VK_IMAGE_VIEW_TYPE_MAX_ENUM),
	_allocator(VK_NULL_HANDLE),
	_allocation(VK_NULL_HANDLE), 
	_ID(UINT32_MAX)
{

}

GPUImage::GPUImage(VkDevice device,
	VkImage image,
	VkImageView imageView,
	VkImageUsageFlags usage,
	VkImageViewType imageViewType,
	VmaAllocator allocator,
	VmaAllocation allocation)
	: _device(device),
	_image(image), 
	_imageView(imageView), 
	_usage(usage),
	_imageViewType(imageViewType),
	_allocator(allocator), 
	_allocation(allocation), 
	_ID(UINT32_MAX)
{
	_ID = MakeImageID();
}

GPUImage::~GPUImage()
{
	if (_image != VK_NULL_HANDLE)
	{
		//vkDestroyImageView(_device, _imageView, nullptr);
		vmaDestroyImage(_allocator, _image, _allocation);
	}
}

VkImage GPUImage::GetImageHandle()
{
	return _image;
}

VkImageView GPUImage::GetImageViewHandle()
{
	return _imageView;
}

VkImageUsageFlags GPUImage::GetImageUsage()
{
	return _usage;
}

VkImageViewType GPUImage::GetImageViewType()
{
	return _imageViewType;
}

uint32_t GPUImage::GetID()
{
	return _ID;
}

uint32_t MakeImageID()
{
	static uint32_t idCounter = 0;
	uint32_t id = idCounter;
	++idCounter;
	return id;
}

}