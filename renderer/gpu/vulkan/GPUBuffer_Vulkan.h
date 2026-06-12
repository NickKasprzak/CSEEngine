#pragma once
#include "refcount/RefCounted.h"
#include "volk.h"
#include "vk_mem_alloc.h"
#include <cstdint>

namespace CSERenderer
{

class GPUBuffer : public CSECore::RefCounted
{
public:
	GPUBuffer();
	GPUBuffer(VkBuffer buffer,
		VkBufferUsageFlags usage,
		VkDeviceSize size,
		VmaAllocator allocator,
		VmaAllocation allocation);
	GPUBuffer(const GPUBuffer& other) = delete;
	virtual ~GPUBuffer();

	void operator=(const GPUBuffer& other) = delete;

	VkBuffer GetHandle();
	VkBufferUsageFlags GetUsage();
	VkDeviceSize GetSize();
	uint32_t GetID();

private:
	VkBuffer _buffer;
	VkBufferUsageFlags _usage;
	VkDeviceSize _size;
	VmaAllocator _allocator;
	VmaAllocation _allocation;
	uint32_t _ID;
};

}