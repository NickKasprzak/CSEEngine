if(USE_VULKAN)
	find_package(Vulkan REQUIRED)

	if (WIN32)
		set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
	endif()

	add_subdirectory(glm)
	add_subdirectory(vk-bootstrap)
	add_subdirectory(volk)
	add_subdirectory(VulkanMemoryAllocator)
endif()