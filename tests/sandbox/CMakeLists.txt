cmake_minimum_required(VERSION 3.20)
project(CSESandbox
		VERSION 0.0.1
		LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

set(SOURCES
	main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC CSEEngine)
target_include_directories(${PROJECT_NAME} PUBLIC "../../engine")
file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/testdir)
file(COPY "testdir" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})