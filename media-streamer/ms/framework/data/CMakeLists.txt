project(ms-framework-data)

add_library(${PROJECT_NAME}
    SHARED
    object.cpp
    decoder.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ms-common
    libdash::libdash)
