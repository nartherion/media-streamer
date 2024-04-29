#pragma once

#include <ms/av/frame.hpp>

#include <cstdint>

namespace ms::media
{

class frame_acceptor
{
public:
    virtual void accept_video(av::frame frame) = 0;
    virtual void accept_audio(av::frame frame) = 0;
    virtual ~frame_acceptor() = default;
};

} // namespace ms::media
