cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

project(media-streamer)

find_package(Microsoft.GSL 4.0.0 REQUIRED)
find_package(spdlog 1.9.2 REQUIRED)
find_package(Libav REQUIRED)

include(cmake/compiler_warnings.cmake)
add_compile_options(${COMPILER_WARNINGS})

add_subdirectory(dependencies)
add_subdirectory(media-streamer)
