#pragma once

#include <ms/framework/mpd/representation_stream.hpp>

#include <vector>

#include <IBaseUrl.h>
#include <IRepresentation.h>
#include <IAdaptationSet.h>
#include <IMPD.h>
#include <IPeriod.h>

namespace ms::framework::mpd
{

class abstract_representation_stream : public representation_stream
{
public:
    abstract_representation_stream(const dash::mpd::IMPD &mpd, const std::vector<dash::mpd::IBaseUrl *> &base_urls);
    virtual ~abstract_representation_stream() = default;

    std::uint32_t get_first_segment_number() const override;
    std::uint32_t get_current_segment_number() const override;
    std::uint32_t get_last_segment_number() const override;

protected:
    const dash::mpd::IMPD &mpd_;
    const std::vector<dash::mpd::IBaseUrl *> base_urls_;
};

} // namespace ms::framework::mpd
