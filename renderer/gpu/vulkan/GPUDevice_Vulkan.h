#pragma once
#include "volk.h"
#include "VkBootstrap.h"
#include "vk_mem_alloc.h"

namespace CSERenderer
{

class GPUDevice_Vulkan
{
public:
	GPUDevice_Vulkan();
	~GPUDevice_Vulkan();

	void Initialize();
	void Dispose();

	const VkInstance GetVkInstance();
	const VkPhysicalDevice GetVkPhysicalDevice();
	const VkDevice GetVkDevice();
	const VmaAllocator GetVMAAllocator();

	uint32_t GetGraphicsQueueFamilyIndex();
	uint32_t GetTransferQueueFamilyIndex();
	uint32_t GetPresentQueueFamilyIndex();

private:
	vkb::Instance _instance;
	vkb::PhysicalDevice _physDevice;
	vkb::Device _device;
	VmaAllocator _vmaAllocator;
};

}