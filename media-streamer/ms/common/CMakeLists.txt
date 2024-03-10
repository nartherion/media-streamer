project(ms-common)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_SOURCE_DIR}/media-streamer)
target_link_libraries(${PROJECT_NAME}
    INTERFACE
    Boost::headers
    spdlog::spdlog
    fmt::fmt)
