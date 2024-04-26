#pragma once

extern "C"
{

#include <libavutil/error.h>

} // extern "C"

#include <string>

namespace ms::av
{

inline std::string make_error_string(const int error)
{
    char buffer[512];
    av_make_error_string(buffer, 512, error);
    return buffer;
}

} // namespace ms::av
