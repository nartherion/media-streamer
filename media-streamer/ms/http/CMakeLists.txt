project(ms-http)
add_library(${PROJECT_NAME} server.cpp session.cpp client.cpp)
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    Boost::headers
    ms-common)
