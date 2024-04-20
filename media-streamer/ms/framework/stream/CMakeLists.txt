project(ms-stream)

add_library(${PROJECT_NAME} SHARED receiver.cpp)
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ms-common
    libdash::libdash)
