#pragma once
#include "../GPUImage.h"
#include "refcount/Ref.h"
#include "volk.h"
#include "vk_mem_alloc.h"
#include <cstdint>

namespace CSERenderer
{

class GPUImage_Vulkan : public GPUImage
{
public:
	GPUImage_Vulkan();
	GPUImage_Vulkan(VkDevice device,
		VkImage image,
		VkImageView imageView,
		VkImageUsageFlags usage,
		VkImageViewType imageViewType,
		VmaAllocator allocator,
		VmaAllocation allocation);
	GPUImage_Vulkan(const GPUImage_Vulkan& other) = delete;
	virtual ~GPUImage_Vulkan();

	void operator=(const GPUImage_Vulkan& other) = delete;

	VkImage GetImageHandle();
	VkImageView GetImageViewHandle();
	VkImageUsageFlags GetImageUsage();
	VkImageViewType GetImageViewType();
	uint32_t GetID();

private:
	VkDevice _device;
	VkImage _image;
	VkImageView _imageView;
	VkImageUsageFlags _usage;
	VkImageViewType _imageViewType;
	VmaAllocator _allocator;
	VmaAllocation _allocation;
	uint32_t _ID;
};

}