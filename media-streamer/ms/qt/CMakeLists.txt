project(ms-qt)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_library(${PROJECT_NAME}
    SHARED
    player.cpp
    gl_renderer.cpp
    audio_player.cpp
    qt_player.ui)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ms-media
    Qt5::Widgets
    Qt5::OpenGL
    Qt5::Gui)
