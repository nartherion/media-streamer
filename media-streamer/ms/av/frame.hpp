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
    frame();

    AVFrame *native();
    void set_codec(codec_context codec);
    std::optional<codec_context> get_codec() const;

private:
    std::optional<codec_context> codec_context_;
    std::shared_ptr<AVFrame> av_frame_;
};

} // namespace ms::av
