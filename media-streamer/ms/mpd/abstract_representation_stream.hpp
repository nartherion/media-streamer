#pragma once

#include <ms/mpd/representation_stream.hpp>

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
    abstract_representation_stream(dash::mpd::IMPD *mpd, dash::mpd::IPeriod *period,
                                   dash::mpd::IAdaptationSet *adaptation_set,
                                   dash::mpd::IRepresentation *representation);

    [[nodiscard]] std::uint32_t get_first_segment_number() const override;
    [[nodiscard]] std::uint32_t get_current_segment_number() const override;
    [[nodiscard]] std::uint32_t get_last_segment_number() const override;

protected:
    virtual void set_base_urls(const std::vector<dash::mpd::IBaseUrl *> base_urls);

    std::vector<dash::mpd::IBaseUrl *> base_urls_;
    dash::mpd::IMPD *mpd_;
    dash::mpd::IPeriod *period_;
    dash::mpd::IAdaptationSet *adaptation_set_;
    dash::mpd::IRepresentation *representation_;
};

} // namespace ms::mpd
