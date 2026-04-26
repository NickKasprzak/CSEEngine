cmake_minimum_required(VERSION 3.20)
project(CSEEngine
		VERSION 0.0.1
		LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES
	EngineApp.h
	EngineApp.cpp

	utility/PlatformWindowInfo_Conversion.h
	utility/PlatformWindowInfo_Conversion.cpp
)

set(INCLUDE_DIRS
	PUBLIC
		../core
		../application
		../renderer
)

set(LIBS
	PUBLIC 
		CSECore
		CSEApp
	PRIVATE
		CSERenderer
)

set(COMP_DEFS
)

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