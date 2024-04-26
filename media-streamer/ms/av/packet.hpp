#pragma once

#include <ms/av/codec_context.hpp>

extern "C"
{

#include <libavcodec/packet.h>

} // extern "C"

#include <memory>

namespace ms::av
{

class packet
{
public:
    packet();

    AVPacket *native();
    const AVPacket *native() const;

    void set_codec(codec_context codec);
    std::optional<codec_context> get_codec() const;

private:
    std::optional<codec_context> codec_context_;
    std::shared_ptr<AVPacket> av_packet_;
};

} // namespace ms::av
