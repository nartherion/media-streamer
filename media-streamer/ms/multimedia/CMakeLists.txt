project(ms-multimedia)

add_library(${PROJECT_NAME}
    SHARED
    stream.cpp
    manager.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC ms-framework-stream)
