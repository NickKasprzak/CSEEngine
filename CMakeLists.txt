cmake_minimum_required(VERSION 3.20)
project(CaveStoryEngine
		VERSION 0.0.1
		LANGUAGES CXX)

option(USE_VULKAN "Build renderer with Vulkan" ON)
option(USE_SDL3 "Build application with SDL3" ON)
option(USE_DEBUG_FEATURES "Build with debug features" ON)
option(DISABLE_EXCEPTIONS "Build with exceptions disabled" ON)
option(BUILD_TESTS "Build with tests enabled" ON)

add_subdirectory(core)
add_subdirectory(renderer)
add_subdirectory(application)
add_subdirectory(engine)

if(BUILD_TESTS)
	add_subdirectory(tests)
endif()
