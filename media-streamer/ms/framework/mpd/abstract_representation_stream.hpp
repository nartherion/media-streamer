#pragma once

#include <ms/framework/mpd/representation_stream.hpp>

#include <vector>

#include <IBaseUrl.h>
#include <IRepresentation.h>
#include <IAdaptationSet.h>
#include <IMPD.h>
#include <IPeriod.h>

namespace ms::mpd
{

class abstract_representation_stream : public representation_stream
{
public:
    abstract_representation_stream(const dash::mpd::IMPD &mpd, const dash::mpd::IPeriod &period,
                                   const dash::mpd::IAdaptationSet &adaptation_set,
                                   const dash::mpd::IRepresentation &representation);
    virtual ~abstract_representation_stream() = default;

    std::uint32_t get_first_segment_number() const override;
    std::uint32_t get_current_segment_number() const override;
    std::uint32_t get_last_segment_number() const override;

protected:
    std::vector<dash::mpd::IBaseUrl *> base_urls_;
    const dash::mpd::IMPD &mpd_;
    const dash::mpd::IPeriod &period_;
    const dash::mpd::IAdaptationSet &adaptation_set_;
    const dash::mpd::IRepresentation &representation_;
};

} // namespace ms::mpd
