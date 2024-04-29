#include <ms/av/frame.hpp>

namespace ms::av
{

namespace
{

void free_av_frame(AVFrame *av_frame)
{
    av_frame_free(&av_frame);
}

} // namespace

frame::frame()
    : av_frame_(av_frame_alloc(), free_av_frame)
{}

AVFrame *frame::native()
{
    return av_frame_.get();
}

void frame::set_codec(codec_context codec)
{
    codec_context_ = codec;
}

std::optional<codec_context> frame::get_codec() const
{
    return codec_context_;
}

} // namespace ms::av
