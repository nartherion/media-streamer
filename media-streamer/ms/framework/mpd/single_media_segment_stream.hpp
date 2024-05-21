#pragma once

#include <ms/framework/mpd/abstract_representation_stream.hpp>

#include <IRepresentation.h>

namespace ms::framework::mpd
{

class single_media_segment_stream final : public abstract_representation_stream
{
public:
    single_media_segment_stream(std::shared_ptr<const dash::mpd::IMPD> mpd, const std::vector<dash::mpd::IBaseUrl *> &base_urls,
                                const dash::mpd::IRepresentation &representation);

    std::shared_ptr<dash::mpd::ISegment> get_initialization_segment() const override;
    std::shared_ptr<dash::mpd::ISegment> get_index_segment(std::size_t segment_number) const override;
    std::shared_ptr<dash::mpd::ISegment> get_media_segment(std::size_t segment_number) const override;
    std::shared_ptr<dash::mpd::ISegment> get_bitstream_switching_segment() const override;
    type get_stream_type() const override;
    std::uint32_t get_size() const override;
    std::uint32_t get_first_segment_number() const override;
    std::uint32_t get_current_segment_number() const override;
    std::uint32_t get_last_segment_number() const override;
    std::uint32_t get_average_segment_duration() const override;

private:
    const dash::mpd::IRepresentation &representation_;
};

} // namespace ms::framework::mpd
