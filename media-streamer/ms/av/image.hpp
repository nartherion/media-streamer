#pragma once

#include <ms/av/codec_context.hpp>

extern "C"
{

#include <libavutil/frame.h>

} // extern "C"

#include <memory>

namespace ms::av
{

class image
{
public:
    image(int width, int height, AVPixelFormat format);

    AVFrame *native();

private:
    static void free_av_image(AVFrame *av_image);

    std::unique_ptr<AVFrame, decltype(&free_av_image)> av_image_;
};

} // namespace ms::av
