add_executable(http-server http_server.cpp)
target_link_libraries(http-server PRIVATE ms-http)

add_executable(http-client http_client.cpp)
target_link_libraries(http-client PRIVATE ms-http)
