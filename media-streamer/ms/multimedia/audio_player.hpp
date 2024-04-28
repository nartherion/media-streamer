#pragma once

#include <ms/media/frame_acceptor.hpp>

namespace ms::multimedia
{

class audio_player
{
public:
    virtual void play(media::audio_frame frame);
    virtual ~audio_player() = default;
};

} // namespace ms::multimedia
