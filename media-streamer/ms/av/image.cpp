#include <ms/av/image.hpp>

extern "C"
{

#include <libavutil/imgutils.h>

} // extern "C"

namespace ms::av
{

namespace
{

AVFrame *make_av_image(const int width, const int height, const AVPixelFormat format)
{
    AVFrame *frame = av_frame_alloc();

    frame->width = width;
    frame->height = height;
    frame->format = format;

    av_image_alloc(frame->data, frame->linesize, width, height, format, 1);
    return frame;
}

} // namespace

image::image(const int width, const int height, const AVPixelFormat format)
    : av_image_(make_av_image(width, height, format), free_av_image)
{}

AVFrame *image::native()
{
    return av_image_.get();
}

void image::free_av_image(AVFrame *av_frame)
{
    av_freep(&av_frame->data[0]);
    av_frame_free(&av_frame);
}

} // namespace ms::av
