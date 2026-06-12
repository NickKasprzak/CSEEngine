#pragma once
#include "refcount/RefCounted.h"
#include "volk.h"
#include "vk_mem_alloc.h"
#include <cstdint>

namespace CSERenderer
{

class GPUImage : public CSECore::RefCounted
{
public:
	GPUImage();
	GPUImage(VkDevice device,
		VkImage image,
		VkImageView imageView,
		VkImageUsageFlags usage,
		VkImageViewType imageViewType,
		VmaAllocator allocator,
		VmaAllocation allocation);
	GPUImage(const GPUImage& other) = delete;
	virtual ~GPUImage();

	void operator=(const GPUImage& other) = delete;

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