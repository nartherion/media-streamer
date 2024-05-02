#include <ms/av/frame.hpp>

namespace ms::av
{

frame::frame()
    : av_frame_(av_frame_alloc(), free_av_frame)
{}

AVFrame *frame::native()
{
    return av_frame_.get();
}

void frame::free_av_frame(AVFrame *av_frame)
{
    av_frame_free(&av_frame);
}

} // namespace ms::av
