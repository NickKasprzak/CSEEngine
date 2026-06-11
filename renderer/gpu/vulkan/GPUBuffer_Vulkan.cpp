#include "GPUBuffer_Vulkan.h"
#include "Logger.h"
#include "CSEAssert.h"

namespace CSERenderer
{

uint32_t MakeBufferID();

GPUBuffer_Vulkan::GPUBuffer_Vulkan()
	: _buffer(VK_NULL_HANDLE), _usage(0), _size(0), _allocator(VK_NULL_HANDLE), _allocation(VK_NULL_HANDLE), _ID(UINT32_MAX)
{

}

GPUBuffer_Vulkan::GPUBuffer_Vulkan(VkBuffer buffer,
	VkBufferUsageFlags usage,
	VkDeviceSize size,
	VmaAllocator allocator,
	VmaAllocation allocation)
	: _buffer(buffer), _usage(usage), _size(size), _allocator(allocator), _allocation(allocation), _ID(UINT32_MAX)
{
	_ID = MakeBufferID();
}

GPUBuffer_Vulkan::~GPUBuffer_Vulkan()
{
	if (_buffer != VK_NULL_HANDLE)
	{
		vmaDestroyBuffer(_allocator, _buffer, _allocation);
	}
}

VkBuffer GPUBuffer_Vulkan::GetHandle()
{
	return _buffer;
}

VkBufferUsageFlags GPUBuffer_Vulkan::GetUsage()
{
	return _usage;
}

VkDeviceSize GPUBuffer_Vulkan::GetSize()
{
	return _size;
}

uint32_t MakeBufferID()
{
	static uint32_t idCounter = 0;
	uint32_t id = idCounter;
	++idCounter;
	return id;
}

}