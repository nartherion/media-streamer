#pragma once

#include <ms/decoder/frame_acceptor.hpp>

namespace ms::framework::data
{

class decoder_events_handler : public media::frame_acceptor
{
public:
    virtual void on_decoding_finished() = 0;
};

} // namespace ms::framework::data
