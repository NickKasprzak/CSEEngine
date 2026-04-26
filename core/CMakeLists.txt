cmake_minimum_required(VERSION 3.20)
project(CSECore
		VERSION 0.0.1
		LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES
	Logger.h
	Logger.cpp
	Any.h
	CSEAssert.h
	CSEAssert.cpp
	Expected.h
	Utility.h

	filesystem/FileSystem.h
	filesystem/FileSystem.cpp
	filesystem/FileNavigator.h
	filesystem/File.h
	filesystem/navigators/FileNavigator_Device.h

	eventsystem/EventTraits.h
	eventsystem/EventCallback.h
	eventsystem/EventCallbackList.h
	eventsystem/EventQueue.h
	eventsystem/EventDistributor.h

	memory/MemoryUtils.h

	containers/LinkedArray.h
	containers/SkipArray.h

	refcount/Ref.h
	refcount/RefCounted.h
)

set(COMP_DEFS
)

if(WIN32)
	set(WINDOWS_SOURCES
		filesystem/navigators/FileNavigator_Win32.h
		filesystem/navigators/FileNavigator_Win32.cpp
		filesystem/files/File_Win32.h
		filesystem/files/File_Win32.cpp
	)

	list(APPEND SOURCES ${WINDOWS_SOURCES})

endif()

if(USE_DEBUG_FEATURES)
	list(APPEND COMP_DEFS
		PUBLIC
			ASSERTS_ENABLED=1
	)
endif()

if(DISABLE_EXCEPTIONS)
	if (MSVC)
		list(APPEND COMP_DEFS
			PUBLIC
				"_HAS_EXCEPTIONS=0"
		)
	endif()
endif()

add_library(${PROJECT_NAME} ${SOURCES})
target_compile_definitions(${PROJECT_NAME} ${COMP_DEFS})