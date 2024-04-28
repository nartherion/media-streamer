#pragma once

#include <ms/media/frame_acceptor.hpp>
#include <ms/framework/data/object.hpp>

#include <IRepresentation.h>

namespace ms::framework::data
{

class decoder_events_handler
{
public:
    virtual std::shared_ptr<data::object> find_initialization_segment(
        const dash::mpd::IRepresentation &representation) const = 0;
    virtual std::shared_ptr<data::object> get_oldest_segment() = 0;
    virtual ~decoder_events_handler() = default;
};

} // namespace ms::framework::data
