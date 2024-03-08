cmake_minimum_required(VERSION 3.28)

add_library(http-server server.cpp session.cpp)

target_link_libraries(http-server PUBLIC Boost::headers)
