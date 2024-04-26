#pragma once

#include <ms/decoder/frame_acceptor.hpp>
#include <ms/framework/data/object.hpp>

#include <IRepresentation.h>

namespace ms::framework::data
{

class decoder_events_handler : public media::frame_acceptor
{
public:
    virtual void on_decoding_finished() = 0;
    virtual std::shared_ptr<data::object> find_initialization_segment(
        const dash::mpd::IRepresentation &representation) = 0;
    virtual std::shared_ptr<data::object> get_next_segment() = 0;
};

} // namespace ms::framework::data
