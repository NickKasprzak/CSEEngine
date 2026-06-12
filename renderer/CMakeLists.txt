cmake_minimum_required(VERSION 3.20)
project(CSERenderer
		VERSION 0.0.1
		LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(thirdparty)

set(SOURCES
	RenderContext.h
	RenderContext.cpp

	gpu/GPUBackend.h
	gpu/GPUBackend.cpp
	gpu/GPUBackend_Impl.h
	gpu/GPUBufferCreateInfo.h
	gpu/GPUDataLayout.h
	gpu/GPUDataLayout.cpp
	gpu/GPUDataLayoutRegistry.h
	gpu/GPUDataLayoutRegistry.cpp
	gpu/GPUImageCreateInfo.h
	gpu/GPUImageFormats.h
	gpu/GPUPipelineCreateInfo.h

	utility/PlatformWindowInfo_Renderer.h
)

set(INCLUDE_DIRS
	PUBLIC
		../core
)

set(LIBS
	PUBLIC
		CSECore
)

set(COMP_DEFS
)

if(USE_VULKAN)

	set(VULKAN_SOURCES
		gpu/vulkan/internal/Config_Vulkan.h
		gpu/vulkan/internal/DescriptorPool_Vulkan.h
		gpu/vulkan/internal/DescriptorPool_Vulkan.cpp
		gpu/vulkan/internal/GPUPipelineBuilder_Vulkan.h
		gpu/vulkan/internal/GPUPipelineBuilder_Vulkan.cpp
		gpu/vulkan/internal/GPUPipelineRegistry_Vulkan.h
		gpu/vulkan/internal/GPUPipelineRegistry_Vulkan.cpp
		gpu/vulkan/internal/RenderGraph_Vulkan.h
		gpu/vulkan/internal/ShaderProcessor_Vulkan.h
		gpu/vulkan/internal/ShaderProcessor_Vulkan.cpp
		gpu/vulkan/internal/SSBOAllocator_Vulkan.h
		gpu/vulkan/internal/SSBOAllocator_Vulkan.cpp
		gpu/vulkan/internal/Utils_Vulkan.h
		gpu/vulkan/internal/Vertex_Vulkan.h

		gpu/vulkan/GPUBackend_Vulkan.h
		gpu/vulkan/GPUBackend_Vulkan.cpp
		gpu/vulkan/GPUBuffer_Vulkan.h
		gpu/vulkan/GPUBuffer_Vulkan.cpp
		gpu/vulkan/GPUDevice_Vulkan.h
		gpu/vulkan/GPUDevice_Vulkan.cpp
		gpu/vulkan/GPUImage_Vulkan.h
		gpu/vulkan/GPUImage_Vulkan.cpp
		gpu/vulkan/GPUPipelineData_Vulkan.h
		gpu/vulkan/GPUPipelineData_Vulkan.cpp
		gpu/vulkan/GPUPipeline_Vulkan.h
		gpu/vulkan/GPUPipeline_Vulkan.cpp
	)

	set(VULKAN_THIRD_PARTY_SOURCES
		thirdparty/SPIRV-Reflect/spirv_reflect.h
		thirdparty/SPIRV-Reflect/spirv_reflect.cpp
	)

	list(APPEND SOURCES ${VULKAN_SOURCES})
	list(APPEND SOURCES ${VULKAN_THIRD_PARTY_SOURCES})

	list(APPEND INCLUDE_DIRS
		PRIVATE
			thirdparty/glm
			thirdparty/vk-bootstrap
			thirdparty/volk
			thirdparty/VulkanMemoryAllocator/include
			thirdparty/SPIRV-Reflect
	)

	list(APPEND LIBS
		PRIVATE
			volk::volk
			vk-bootstrap::vk-bootstrap
			GPUOpen::VulkanMemoryAllocator
			glm::glm
	)

	list(APPEND COMP_DEFS
		PRIVATE
			"VULKAN_BACKEND"
			"VMA_STATIC_VULKAN_FUNCTIONS=0"
			"VMA_DYNAMIC_VULKAN_FUNCTIONS=0"
	)

endif()

if(USE_DEBUG_FEATURES)
	list(APPEND COMP_DEFS
		PUBLIC
			RENDER_DEBUG_ENABLED
	)
endif()

if(DISABLE_EXCEPTIONS)
	if (MSVC)
		list(APPEND COMP_DEFS
			PRIVATE
				"_HAS_EXCEPTIONS=0"
		)
	endif()
endif()

add_library(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} ${INCLUDE_DIRS})
target_compile_definitions(${PROJECT_NAME} ${COMP_DEFS})
target_link_libraries(${PROJECT_NAME} ${LIBS})