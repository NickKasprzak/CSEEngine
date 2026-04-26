#pragma once
#include "../GPUBuffer.h"
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"

namespace CSERenderer
{

struct VulkanBufferInfo
{
	VkBufferUsageFlags usage;
	VkDeviceSize size;
	VmaAllocationCreateFlags alloc;
};

class GPUBuffer_Vulkan : public GPUBuffer
{
public:
	GPUBuffer_Vulkan();
	GPUBuffer_Vulkan(VulkanBufferInfo* params, uint32_t queueFamily, VmaAllocator allocator);
	GPUBuffer_Vulkan(const GPUBuffer_Vulkan& other) = delete;
	virtual ~GPUBuffer_Vulkan();

	void operator=(const GPUBuffer_Vulkan& other) = delete;

	VkBuffer GetHandle();
	VkBufferUsageFlags GetUsage();
	VkDeviceSize GetSize();
	
	void MapBuffer();
	void UnmapBuffer();
	void* GetMapping();

private:
	VkBuffer _buffer;
	VulkanBufferInfo _info;
	VmaAllocator _allocator;
	VmaAllocation _allocation;
	void* _mapping;
};

}