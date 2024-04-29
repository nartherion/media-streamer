#pragma once

#include <ms/av/frame.hpp>

namespace ms::multimedia
{

class audio_player
{
public:
    virtual void play(av::frame frame);
    virtual ~audio_player() = default;
};

} // namespace ms::multimedia
