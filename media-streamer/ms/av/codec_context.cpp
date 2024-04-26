#include <ms/av/codec_context.hpp>
#include <ms/av/error.hpp>

#include <spdlog/spdlog.h>

namespace ms::av
{

namespace
{

void free_av_codec_context(AVCodecContext *av_codec_context)
{
    avcodec_free_context(&av_codec_context);
}

} // namespace

std::optional<codec_context> codec_context::create(const AVCodec *av_codec,
                                                   const AVCodecParameters *av_codec_parameters,
                                                   const AVStream *av_stream)
{
    AVCodecContext *av_codec_context = avcodec_alloc_context3(av_codec);

    if (const int error = avcodec_parameters_to_context(av_codec_context, av_codec_parameters); error < 0)
    {
        SPDLOG_ERROR("Failed to set codec parameters: {}", make_error_string(error));
        return {};
    }

    if (const int error = avcodec_open2(av_codec_context, NULL, NULL); error < 0)
    {
        SPDLOG_ERROR("Failed to open codec: {}", make_error_string(error));
        return {};
    }

    return codec_context(av_codec_context, av_stream);
}

AVCodecContext *codec_context::native()
{
    return av_codec_context_.get();
}

const AVStream *codec_context::stream() const
{
    return av_stream_;
}

codec_context::codec_context(AVCodecContext *av_codec_context, const AVStream *av_stream)
    : av_stream_(av_stream),
      av_codec_context_(av_codec_context, free_av_codec_context)
{}

} // namespace ms::av
