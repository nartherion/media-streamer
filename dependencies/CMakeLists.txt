set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

add_subdirectory(libdash/libdash)

target_compile_options(dash PRIVATE -w)
target_include_directories(dash SYSTEM PUBLIC libdash/libdash/libdash/include)
add_library(libdash::libdash ALIAS dash)

target_compile_options(libdash_networkpart_test PRIVATE -w)

set(CMAKE_WARN_DEPRECATED ON CACHE BOOL "" FORCE)
