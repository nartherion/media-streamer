#include <ms/av/frame.hpp>

namespace ms::av
{

frame::frame(const codec_context codec)
    : codec_context_(codec),
      av_frame_(av_frame_alloc(), free_av_frame)
{}

AVFrame *frame::native()
{
    return av_frame_.get();
}

codec_context frame::codec() const
{
    return codec_context_;
}

void frame::free_av_frame(AVFrame *av_frame)
{
    av_frame_free(&av_frame);
}

} // namespace ms::av
