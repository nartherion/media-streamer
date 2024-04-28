#pragma once

#include <ms/media/frame_acceptor.hpp>

namespace ms::multimedia
{

class video_renderer
{
public:
    virtual void render(media::video_frame frame);
    virtual ~video_renderer() = default;
};

} // namespace ms::multimedia
