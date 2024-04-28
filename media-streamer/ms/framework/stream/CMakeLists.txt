project(ms-framework-stream)

add_library(${PROJECT_NAME}
    SHARED
    receiver.cpp
    manager.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC ms-framework-data)
