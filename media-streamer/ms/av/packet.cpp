#include <ms/av/packet.hpp>

namespace ms::av
{

namespace
{

void free_av_packet(AVPacket *av_packet)
{
    av_packet_free(&av_packet);
}

} // namespace

packet::packet()
    : av_packet_(av_packet_alloc(), free_av_packet)
{}

AVPacket *packet::native()
{
    return av_packet_.get();
}

const AVPacket *packet::native() const
{
    return av_packet_.get();
}

void packet::set_codec(const codec_context codec)
{
    codec_context_ = codec;
}

std::optional<codec_context> packet::get_codec() const
{
    return codec_context_;
}

} // namespace ms::av
