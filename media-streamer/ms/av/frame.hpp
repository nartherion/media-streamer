#pragma once

#include <ms/av/codec_context.hpp>

extern "C"
{

#include <libavutil/frame.h>

} // extern "C"

#include <memory>

namespace ms::av
{

class frame
{
public:
    frame(codec_context codec);

    AVFrame *native();
    codec_context codec() const;

private:
    static void free_av_frame(AVFrame *av_frame);

    const codec_context codec_context_;
    std::unique_ptr<AVFrame, decltype(&free_av_frame)> av_frame_;
};

} // namespace ms::av
