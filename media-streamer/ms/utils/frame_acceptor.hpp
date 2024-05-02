#pragma once

#include <ms/av/frame.hpp>

namespace ms::utils
{

class frame_acceptor
{
public:
    virtual void accept(av::frame frame) = 0;
    virtual ~frame_acceptor() = default;
};

} // namespace ms::utils
