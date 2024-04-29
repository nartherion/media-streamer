project(ms-common)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${MEDIA_STREAMER_ROOT}/media-streamer)
target_link_libraries(${PROJECT_NAME}
    INTERFACE
    spdlog::spdlog
    Microsoft.GSL::GSL)
