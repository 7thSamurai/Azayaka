cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

project(Azayaka VERSION 1.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")

find_package(PNG)
if (PNG_FOUND)
	add_definitions(-DUSE_PNG)
endif()

find_package(OpenGL)
if (OpenGL_FOUND)
	add_definitions(-DUSE_GL)
endif()

include_directories(src)

find_package(SDL2)
if (SDL2_FOUND)
	add_subdirectory(src/sdl)
endif()

add_subdirectory(src/core)
add_subdirectory(src/tester)

file(COPY data DESTINATION bin)
