include(FindPackageHandleStandardArgs)

find_package(PkgConfig REQUIRED QUIET)
if(PKG_CONFIG_FOUND)
    if(NOT LIBAV_AVCODEC)
        pkg_check_modules(_LIBAV_AVCODEC REQUIRED QUIET libavcodec)
    endif()

    if(NOT LIBAV_AVFORMAT)
        pkg_check_modules(_LIBAV_AVFORMAT REQUIRED QUIET libavformat)
    endif()

    if(NOT LIBAV_AVUTIL)
        pkg_check_modules(_LIBAV_AVUTIL REQUIRED QUIET libavutil)
    endif()

    if(NOT LIBAV_SWSCALE)
        pkg_check_modules(_LIBAV_SWSCALE REQUIRED QUIET libswscale)
    endif()
endif()

find_library(LIBAV_AVCODEC
    NAMES avcodec
    PATHS ${_LIBAV_AVCODEC_LIBRARY_DIRS})

find_library(LIBAV_AVFORMAT
    NAMES avformat
    PATHS ${_LIBAV_AVFORMAT_LIBRARY_DIRS})

find_library(LIBAV_AVUTIL
    NAMES avutil
    PATHS ${_LIBAV_AVUTIL_LIBRARY_DIRS})

find_library(LIBAV_SWSCALE
    NAMES swscale
    PATHS ${_LIBAV_SWSCALE_LIBRARY_DIRS})

find_package_handle_standard_args(Libav DEFAULT_MSG LIBAV_AVCODEC LIBAV_AVFORMAT LIBAV_AVUTIL LIBAV_SWSCALE)

set(LIBAV_LIBRARIES ${LIBAV_AVCODEC} ${LIBAV_AVFORMAT} ${LIBAV_AVUTIL} ${LIBAV_SWSCALE})
