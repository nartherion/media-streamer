#pragma once

#include <ms/framework/mpd/abstract_representation_stream.hpp>

#include <cstdint>

#include <vector>

namespace ms::framework::mpd
{

class segment_template_stream final : public abstract_representation_stream
{
public:
    segment_template_stream(const dash::mpd::IMPD &mpd, const std::vector<dash::mpd::IBaseUrl *> &base_urls,
                            const dash::mpd::IRepresentation &representation,
                            const dash::mpd::ISegmentTemplate &segment_template);

    std::shared_ptr<dash::mpd::ISegment> get_initialization_segment() const override;
    std::shared_ptr<dash::mpd::ISegment> get_index_segment(std::size_t segment_number) const override;
    std::shared_ptr<dash::mpd::ISegment> get_media_segment(std::size_t segment_number) const override;
    std::shared_ptr<dash::mpd::ISegment> get_bitstream_switching_segment() const override;
    type get_stream_type() const override;
    std::uint32_t get_size() const override;
    std::uint32_t get_average_segment_duration() const override;

private:
    const dash::mpd::IRepresentation &representation_;
    const dash::mpd::ISegmentTemplate &segment_template_;
    const std::vector<std::uint32_t> segment_start_times_;
};

} // namespace ms::framework::mpd
