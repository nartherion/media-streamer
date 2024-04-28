project(ms-multimedia)
add_library(${PROJECT_NAME}
    SHARED
    stream.cpp
    manager.hpp)
target_link_libraries(${PROJECT_NAME} PUBLIC ms-framework-stream)
