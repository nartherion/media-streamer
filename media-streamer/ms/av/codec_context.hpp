#pragma once

extern "C"
{

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

} // extern "C"

#include <memory>
#include <optional>

namespace ms::av
{

class codec_context
{
public:
    static std::optional<codec_context> create(const AVCodec *codec, const AVCodecParameters *parameters,
                                               const AVStream *stream);

    AVCodecContext *native();
    const AVCodecContext *native() const;
    const AVStream *stream() const;

private:
    codec_context(AVCodecContext *av_codec_context, const AVStream *av_stream);

    const AVStream *av_stream_;
    std::shared_ptr<AVCodecContext> av_codec_context_;
};

} // namespace ms::av
