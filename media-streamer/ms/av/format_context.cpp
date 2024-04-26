#include <ms/av/format_context.hpp>
#include <ms/av/utils.hpp>

extern "C"
{

#include <libavformat/avio.h>

} // extern "C"

#include <spdlog/spdlog.h>

namespace ms::av
{

std::optional<format_context> format_context::create(const int buffer_size, void *const opaque,
                                                     const read_packet_callback callback)
{
    auto *const buffer = reinterpret_cast<std::uint8_t *>(av_malloc(static_cast<std::size_t>(buffer_size)));

    AVIOContext *const av_io_context = avio_alloc_context(buffer, buffer_size, 0, opaque, callback, NULL, NULL);
    av_io_context->seekable = 0;

    AVFormatContext *av_format_context = avformat_alloc_context();
    av_format_context->pb = av_io_context;

    if (const int error = avformat_open_input(&av_format_context, "", NULL, NULL); error < 0)
    {
        SPDLOG_ERROR("Failed to open input: {}", error);
        return {};
    }

    return format_context(av_format_context);
}

format_context::format_context(AVFormatContext *av_format_context)
    : av_format_context_(av_format_context, close_input)
{}

AVFormatContext *format_context::native()
{
    return av_format_context_.get();
}

void format_context::close_input(AVFormatContext *av_format_context)
{
    avformat_close_input(&av_format_context);
}

} // namespace ms::av
