project(ms-media)
add_library(${PROJECT_NAME} SHARED decoder.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ms-av)
