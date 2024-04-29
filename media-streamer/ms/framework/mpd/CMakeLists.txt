project(ms-framework-mpd)

add_library(${PROJECT_NAME}
    SHARED
    time_utils.cpp
    abstract_representation_stream.cpp
    adaptation_set_stream.cpp
    segment_list_stream.cpp
    segment_template_stream.cpp
    single_media_segment_stream.cpp
    representation_stream_factory.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ms-common
    libdash::libdash)
