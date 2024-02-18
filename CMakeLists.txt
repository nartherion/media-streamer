cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)

project(media-streamer)

find_package(Boost 1.84.0 REQUIRED headers)

include(cmake/compiler_warnings.cmake)
add_compile_options(${COMPILER_WARNINGS})
