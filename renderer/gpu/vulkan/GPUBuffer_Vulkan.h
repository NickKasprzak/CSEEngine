#pragma once
#include "../GPUBuffer.h"
#include "refcount/Ref.h"
#include "volk.h"
#include "vk_mem_alloc.h"
#include <cstdint>

namespace CSERenderer
{

struct VulkanBufferInfo
{
	VkBufferUsageFlags usage;
	VkDeviceSize size;
	VmaAllocationCreateFlags alloc;
};

class GPUBufferView_Vulkan
{
public:
	
private:

};

class GPUBuffer_Vulkan : public GPUBuffer
{
public:
	GPUBuffer_Vulkan();
	GPUBuffer_Vulkan(VkBuffer buffer,
		GPUBufferView_Vulkan bufferView,
		VulkanBufferInfo& info,
		VmaAllocator allocator,
		VmaAllocation allocation,
		uint32_t ID);
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
	GPUBufferView_Vulkan _bufferView;
	VulkanBufferInfo _info;
	VmaAllocator _allocator;
	VmaAllocation _allocation;
	uint32_t _ID;
	void* _mapping;
};

// TODO: Move to whatever is pooling GPUBuffers
CSECore::Ref<GPUBuffer> CreateBuffer_Vulkan(VulkanBufferInfo* params, uint32_t queueFamily, VmaAllocator allocator);

}