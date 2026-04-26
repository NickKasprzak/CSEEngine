#include "GPUBuffer_Vulkan.h"
#include "Logger.h"
#include "CSEAssert.h"

namespace CSERenderer
{

GPUBuffer_Vulkan::GPUBuffer_Vulkan()
	: _buffer(VK_NULL_HANDLE), _info(), _allocator(VK_NULL_HANDLE), _allocation(VK_NULL_HANDLE), _mapping(nullptr)
{

}

GPUBuffer_Vulkan::GPUBuffer_Vulkan(VulkanBufferInfo* params, uint32_t queueFamily, VmaAllocator allocator)
	: _buffer(VK_NULL_HANDLE), _info(), _allocator(VK_NULL_HANDLE), _allocation(VK_NULL_HANDLE), _mapping(nullptr)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.usage = params->usage;
	bufferCreateInfo.size = params->size;
	bufferCreateInfo.queueFamilyIndexCount = 1;
	bufferCreateInfo.pQueueFamilyIndices = &queueFamily;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocCreateInfo{};
	allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocCreateInfo.flags = params->alloc;

	VkBuffer buffer;
	VmaAllocation allocation;
	VkResult result = vmaCreateBuffer(allocator, &bufferCreateInfo, &allocCreateInfo, &buffer, &allocation, nullptr);
	if (result != VK_SUCCESS)
	{
		switch (result)
		{
		default:
			CSE_LOGE("Failed to create buffer. Reason: Unknown error.");
			return;
		}
	}

	_buffer = buffer;
	_info = *params;
	_allocator = allocator;
	_allocation = allocation;
}

GPUBuffer_Vulkan::~GPUBuffer_Vulkan()
{
	if (_buffer != VK_NULL_HANDLE)
	{
		UnmapBuffer();
		vmaDestroyBuffer(_allocator, _buffer, _allocation);
	}
}

VkBuffer GPUBuffer_Vulkan::GetHandle()
{
	return _buffer;
}

VkBufferUsageFlags GPUBuffer_Vulkan::GetUsage()
{
	return _info.usage;
}

VkDeviceSize GPUBuffer_Vulkan::GetSize()
{
	return _info.size;
}

void GPUBuffer_Vulkan::MapBuffer()
{
	CSE_ASSERT(_info.alloc & (VMA_ALLOCATION_CREATE_MAPPED_BIT || VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT), "Buffer being mapped lacks correct allocation flags.");
	CSE_ASSERT(_info.usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT, "Buffer being mapped lacks correct usage flags.");

	VkResult result = vmaMapMemory(_allocator, _allocation, &_mapping);
	if (result != VK_SUCCESS)
	{
		switch (result)
		{
		default:
			CSE_LOGE("Failed to map buffer memory. Reason: Unknown error.");
			return;
		}
	}
}

void GPUBuffer_Vulkan::UnmapBuffer()
{
	if (_mapping != nullptr)
	{
		vmaUnmapMemory(_allocator, _allocation);
		_mapping = nullptr;
	}
}

void* GPUBuffer_Vulkan::GetMapping()
{
	return _mapping;
}

}