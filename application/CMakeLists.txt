cmake_minimum_required(VERSION 3.20)
project(CSEApp
		VERSION 0.0.1
		LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES
	application/App.h
	application/App.cpp
	application/AppPlatform.h
	application/AppPlatform_Impl.h

	window/Window.h
	window/Window_Impl.h

	input/InputManager.h
	input/InputManager.cpp
	input/Keyboard.h
	input/Mouse.h
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

if(USE_SDL3)
	
	find_package(SDL3 REQUIRED)

	set(SDL3_SOURCES
		application/AppPlatform_SDL3.h
		application/AppPlatform_SDL3.cpp

		window/Window_SDL3.h
		window/Window_SDL3.cpp
	)

	list(APPEND SOURCES ${SDL3_SOURCES})

	list(APPEND LIBS
		PRIVATE
			SDL3::SDL3
	)

	list(APPEND COMP_DEFS
		PRIVATE
			SDL3_APPLICATION
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