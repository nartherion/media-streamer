project(ms-av)

add_library(${PROJECT_NAME}
    SHARED
    codec_context.cpp
    format_context.cpp
    frame.cpp
    image.cpp
    packet.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ms-common
    ${LIBAV_LIBRARIES})
