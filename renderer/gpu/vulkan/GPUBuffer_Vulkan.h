#pragma once
#include "../GPUBuffer.h"
#include "refcount/Ref.h"
#include "volk.h"
#include "vk_mem_alloc.h"
#include <cstdint>

namespace CSERenderer
{

class GPUBuffer_Vulkan : public GPUBuffer
{
public:
	GPUBuffer_Vulkan();
	GPUBuffer_Vulkan(VkBuffer buffer,
		VkBufferUsageFlags usage,
		VkDeviceSize size,
		VmaAllocator allocator,
		VmaAllocation allocation);
	GPUBuffer_Vulkan(const GPUBuffer_Vulkan& other) = delete;
	virtual ~GPUBuffer_Vulkan();

	void operator=(const GPUBuffer_Vulkan& other) = delete;

	VkBuffer GetHandle();
	VkBufferUsageFlags GetUsage();
	VkDeviceSize GetSize();

private:
	VkBuffer _buffer;
	VkBufferUsageFlags _usage;
	VkDeviceSize _size;
	VmaAllocator _allocator;
	VmaAllocation _allocation;
	uint32_t _ID;
};

}