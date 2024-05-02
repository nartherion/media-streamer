#pragma once

#include <ms/presentation/frame_renderer.hpp>

namespace ms::qt
{

class audio_player : public presentation::frame_renderer
{
public:
    void render(av::frame frame) override;
};

} // namespace ms::qt
