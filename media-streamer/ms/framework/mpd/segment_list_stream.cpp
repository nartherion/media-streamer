#include <ms/framework/mpd/segment_list_stream.hpp>

namespace ms::framework::mpd
{

segment_list_stream::segment_list_stream(const dash::mpd::IMPD &mpd,
                                         const std::vector<dash::mpd::IBaseUrl *> &base_urls,
                                         const dash::mpd::ISegmentList &segment_list)
    : abstract_representation_stream(mpd, base_urls),
      segment_list_(segment_list)
{}

std::shared_ptr<dash::mpd::ISegment> segment_list_stream::get_initialization_segment() const
{
    if (const dash::mpd::IURLType *initialization = segment_list_.GetInitialization())
    {
        return std::shared_ptr<dash::mpd::ISegment>(initialization->ToSegment(base_urls_));
    }

    return {};
}

std::shared_ptr<dash::mpd::ISegment> segment_list_stream::get_index_segment(const std::size_t segment_number) const
{
    const std::vector<dash::mpd::ISegmentURL *>& segment_urls = segment_list_.GetSegmentURLs();
    if (segment_urls.size() > segment_number)
    {
        return std::shared_ptr<dash::mpd::ISegment>(segment_urls.at(segment_number)->ToIndexSegment(base_urls_));
    }

    return {};
}

std::shared_ptr<dash::mpd::ISegment> segment_list_stream::get_media_segment(const std::size_t segment_number) const
{
    const std::vector<dash::mpd::ISegmentURL *>& segment_urls = segment_list_.GetSegmentURLs();
    if (segment_urls.size() > segment_number)
    {
        return std::shared_ptr<dash::mpd::ISegment>(segment_urls.at(segment_number)->ToMediaSegment(base_urls_));
    }

    return {};
}

std::shared_ptr<dash::mpd::ISegment> segment_list_stream::get_bitstream_switching_segment() const
{
    if (const dash::mpd::IURLType *bitrstream_switching = segment_list_.GetBitstreamSwitching())
    {
        return std::shared_ptr<dash::mpd::ISegment>(bitrstream_switching->ToSegment(base_urls_));
    }

    return {};
}

representation_stream::type segment_list_stream::get_stream_type() const
{
    return representation_stream::type::segment_list;
}

std::uint32_t segment_list_stream::get_size() const
{
    return static_cast<std::uint32_t>(segment_list_.GetSegmentURLs().size());
}

std::uint32_t segment_list_stream::get_average_segment_duration() const
{
    return segment_list_.GetDuration();
}

} // namespace ms::framework::mpd
