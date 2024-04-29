cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(MEDIA_STREAMER_ROOT ${CMAKE_SOURCE_DIR})
set(CMAKE_MODULE_PATH "${MEDIA_STREAMER_ROOT}/cmake/modules")

project(media-streamer)

find_package(Microsoft.GSL 4.0.0 REQUIRED)
find_package(spdlog 1.9.2 REQUIRED)
find_package(Qt5 COMPONENTS Widgets Core Gui Multimedia MultimediaWidgets REQUIRED)
find_package(Libav REQUIRED)

include(cmake/compiler_warnings.cmake)
add_compile_options(${COMPILER_WARNINGS})

add_subdirectory(dependencies)
add_subdirectory(media-streamer)
