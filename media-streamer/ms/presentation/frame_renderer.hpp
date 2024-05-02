#pragma once

#include <ms/av/frame.hpp>

namespace ms::presentation
{

class frame_renderer
{
public:
    virtual void render(av::frame frame) = 0;
    virtual ~frame_renderer() = default;
};

} // namespace ms::presentation
