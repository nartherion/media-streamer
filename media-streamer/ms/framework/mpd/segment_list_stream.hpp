#pragma once

#include <ms/framework/mpd/abstract_representation_stream.hpp>

#include <IMPD.h>
#include <IAdaptationSet.h>
#include <IRepresentation.h>

namespace ms::framework::mpd
{

class segment_list_stream final : public abstract_representation_stream
{
public:
    segment_list_stream(const dash::mpd::IMPD &mpd, const std::vector<dash::mpd::IBaseUrl *> &base_urls,
                        const dash::mpd::ISegmentList &segment_list);

    std::shared_ptr<dash::mpd::ISegment> get_initialization_segment() const override;
    std::shared_ptr<dash::mpd::ISegment> get_index_segment(std::size_t segment_number) const override;
    std::shared_ptr<dash::mpd::ISegment> get_media_segment(std::size_t segment_number) const override;
    std::shared_ptr<dash::mpd::ISegment> get_bitstream_switching_segment() const override;
    type get_stream_type() const override;
    std::uint32_t get_size() const override;
    std::uint32_t get_average_segment_duration() const override;

private:
    const dash::mpd::ISegmentList &segment_list_;
};

} // namespace ms::framework::mpd
