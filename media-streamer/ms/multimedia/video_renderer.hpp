#pragma once

#include <ms/av/frame.hpp>

namespace ms::multimedia
{

class video_renderer
{
public:
    virtual void render(av::frame frame);
    virtual ~video_renderer() = default;
};

} // namespace ms::multimedia
