project(ms-media)

add_library(${PROJECT_NAME}
    SHARED
    object.cpp
    buffer.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ms-common
    libdash::libdash)
