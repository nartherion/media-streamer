#pragma once

#include <ms/framework/mpd/representation_stream.hpp>

#include <map>
#include <memory>

#include <IRepresentation.h>
#include <IMPD.h>
#include <IPeriod.h>
#include <IAdaptationSet.h>

namespace ms::framework::mpd
{

class adaptation_set_stream
{
public:
    using representation_stream_table =
        std::map<const dash::mpd::IRepresentation *, std::shared_ptr<representation_stream>>;

    adaptation_set_stream(const dash::mpd::IMPD &mpd, const dash::mpd::IPeriod &period,
                          const dash::mpd::IAdaptationSet &adaptation_set);

    std::shared_ptr<representation_stream> get_representation_stream(const dash::mpd::IRepresentation &representation);

private:
    representation_stream_table representation_stream_table_;
};

} // namespace ms::framework::mpd
