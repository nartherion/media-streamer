#pragma once

#include <ms/framework/mpd/representation_stream.hpp>

#include <memory>

#include <IMPD.h>
#include <IPeriod.h>
#include <IAdaptationSet.h>
#include <IRepresentation.h>

namespace ms::framework::mpd
{

class representation_stream_factory
{
public:
    static std::shared_ptr<representation_stream> create(representation_stream::type type, const dash::mpd::IMPD &mpd,
                                                         const dash::mpd::IPeriod &period,
                                                         const dash::mpd::IAdaptationSet &adaptation_set,
                                                         const dash::mpd::IRepresentation &representation);
};

} // namespace ms::framework::mpd
