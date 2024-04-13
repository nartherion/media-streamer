#pragma once

#include <ms/framework/mpd/abstract_representation_stream.hpp>

#include <IMPD.h>
#include <IAdaptationSet.h>
#include <IRepresentation.h>

namespace ms::mpd
{

class segment_list_stream final : public abstract_representation_stream
{
public:
    segment_list_stream(const dash::mpd::IMPD &mpd, const dash::mpd::IPeriod &period,
                        const dash::mpd::IAdaptationSet &adaptation_set,
                        const dash::mpd::IRepresentation &representation);

    dash::mpd::ISegment *get_initialization_segment() const override;
    dash::mpd::ISegment *get_index_segment(std::size_t segment_number) const override;
    dash::mpd::ISegment *get_media_segment(std::size_t segment_number) const override;
    dash::mpd::ISegment *get_bitstream_switching_segment() const override;
    type get_stream_type() const override;
    std::uint32_t get_size() const override;
    std::uint32_t get_average_segment_duration() const override;

private:
    dash::mpd::ISegmentList *segment_list_;
};

} // namespace ms::mpd
