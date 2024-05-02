project(ms-utils)
add_library(${PROJECT_NAME} SHARED decoder.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC ms-av)
