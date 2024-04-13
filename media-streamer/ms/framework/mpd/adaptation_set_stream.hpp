#pragma once

#include <ms/framework/mpd/representation_stream.hpp>

#include <map>
#include <memory>

#include <IRepresentation.h>
#include <IMPD.h>
#include <IPeriod.h>
#include <IAdaptationSet.h>

namespace ms::mpd
{

class adaptation_set_stream
{
public:
    adaptation_set_stream(const dash::mpd::IMPD &mpd, const dash::mpd::IPeriod &period,
                          const dash::mpd::IAdaptationSet &adaptation_set);

    std::shared_ptr<representation_stream> get_representation_stream(dash::mpd::IRepresentation &representation);

private:
    std::map<dash::mpd::IRepresentation *, std::shared_ptr<representation_stream>> representations_;
};

} // namespace ms::mpd
