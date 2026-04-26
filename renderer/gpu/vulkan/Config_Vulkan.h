#pragma once
#include "vulkan/vulkan.h"

namespace CSERenderer
{

const static int VULKAN_VERSION_MAJOR = 1;
const static int VULKAN_VERSION_MINOR = 3;
const static int VULKAN_VERSION_PATCH = 0;

static const char* REQUIRED_PHYSICAL_EXTENSIONS[]
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

inline const VkPhysicalDeviceFeatures GetRequiredVulkanFeatures()
{
	VkPhysicalDeviceFeatures features{};
	
	return features;
}

inline const VkPhysicalDeviceVulkan11Features GetRequiredVulkan11Features()
{
	VkPhysicalDeviceVulkan11Features features{};
	features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;

	return features;
}

inline const VkPhysicalDeviceVulkan12Features GetRequiredVulkan12Features()
{
	VkPhysicalDeviceVulkan12Features features{};
	features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

	return features;
}

inline const VkPhysicalDeviceVulkan13Features GetRequiredVulkan13Features()
{
	VkPhysicalDeviceVulkan13Features features{};
	features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	features.dynamicRendering = true;

	return features;
}

const static uint8_t FRAMES_IN_FLIGHT = 2;

}