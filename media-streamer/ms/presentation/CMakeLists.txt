project(ms-presentation)
add_library(${PROJECT_NAME} SHARED manager.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC ms-av)
