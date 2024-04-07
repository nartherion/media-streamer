project(ms-mpd)

add_library(${PROJECT_NAME} abstract_representation_stream.cpp)
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ms-common
    libdash::libdash)
