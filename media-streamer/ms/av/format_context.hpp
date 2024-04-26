#pragma once

extern "C"
{

#include <libavformat/avformat.h>

} // extern "C"

#include <memory>
#include <optional>

#include <gsl/pointers>

namespace ms::av
{

class format_context
{
public:
    using read_packet_callback = int (*)(void *, std::uint8_t *, int);

    static std::optional<format_context> create(int buffer_size, gsl::not_null<void *> opaque, read_packet_callback);

    AVFormatContext *native();

private:
    format_context(AVFormatContext *av_format_context);

    static void close_input(AVFormatContext *av_format_context);

    std::unique_ptr<AVFormatContext, decltype(&close_input)> av_format_context_;
};

} // namespace ms::av
