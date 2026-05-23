#pragma once
#include "../GPUImage.h"
#include "refcount/Ref.h"
#include "volk.h"
#include "vk_mem_alloc.h"
#include <cstdint>

namespace CSERenderer
{

struct VulkanImageInfo
{
	VkImageUsageFlags usage;
	VkFormat format;
	VkExtent3D extent;
	uint32_t mipLevels;
	uint32_t arrayLayers;
	VkSampleCountFlagBits samples;
	VmaAllocationCreateFlags alloc;
	VkImageCreateFlags createFlags;
	VkDevice device;
};

class GPUImage_Vulkan : public GPUImage
{
public:
	GPUImage_Vulkan();
	GPUImage_Vulkan(VkImage image,
		VkImageView imageView,
		VulkanImageInfo& imageInfo,
		VmaAllocator allocator,
		VmaAllocation allocation,
		uint32_t ID);
	GPUImage_Vulkan(const GPUImage_Vulkan& other) = delete;
	virtual ~GPUImage_Vulkan();

	void operator=(const GPUImage_Vulkan& other) = delete;

	VkImage GetImageHandle();
	VkImageView GetImageViewHandle();
	VkImageUsageFlags GetImageUsage();
	VkImageViewType GetImageViewType();
	uint32_t GetID();

	void SetSampler(VkFilter filter, VkSamplerAddressMode addressMode);
	VkSampler GetSampler();

private:
	VkImage _image;
	VkImageView _imageView;
	VulkanImageInfo _info;
	VmaAllocator _allocator;
	VmaAllocation _allocation;
	uint32_t _ID;
	VkSampler _sampler;
};

// TODO: Move to whatever is pooling images
CSECore::Ref<GPUImage> CreateImage_Vulkan(VulkanImageInfo* params, uint32_t queueFamily, VmaAllocator allocator);

}