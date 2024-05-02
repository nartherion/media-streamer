project(ms-media)

add_library(${PROJECT_NAME} SHARED manager.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ms-framework-stream
    ms-presentation)
