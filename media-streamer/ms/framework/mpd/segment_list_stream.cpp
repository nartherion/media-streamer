#include <ms/framework/mpd/segment_list_stream.hpp>

namespace ms::mpd
{

namespace
{

dash::mpd::ISegmentList *find_segment_list(const dash::mpd::IPeriod &period,
                                           const dash::mpd::IAdaptationSet &adaptation_set,
                                           const dash::mpd::IRepresentation &representation)
{
    if (dash::mpd::ISegmentList *segment_list = representation.GetSegmentList())
    {
        return segment_list;
    }

    if (dash::mpd::ISegmentList *segment_list = adaptation_set.GetSegmentList())
    {
        return segment_list;
    }

    if (dash::mpd::ISegmentList *segment_list = period.GetSegmentList())
    {
        return segment_list;
    }

    return nullptr;
}

} // namespace

segment_list_stream::segment_list_stream(const dash::mpd::IMPD &mpd, const dash::mpd::IPeriod &period,
                                         const dash::mpd::IAdaptationSet &adaptation_set,
                                         const dash::mpd::IRepresentation &representation)
    : abstract_representation_stream(mpd, period, adaptation_set, representation),
      segment_list_(find_segment_list(period, adaptation_set, representation))
{}

dash::mpd::ISegment *segment_list_stream::get_initialization_segment() const
{
    if (const dash::mpd::IURLType *initialization = segment_list_->GetInitialization())
    {
        return initialization->ToSegment(base_urls_);
    }

    return nullptr;
}

dash::mpd::ISegment *segment_list_stream::get_index_segment(const std::size_t segment_number) const
{
    const std::vector<dash::mpd::ISegmentURL *>& segment_urls = segment_list_->GetSegmentURLs();
    if (segment_urls.size() > segment_number)
    {
        return segment_urls.at(segment_number)->ToIndexSegment(base_urls_);
    }

    return nullptr;
}

dash::mpd::ISegment *segment_list_stream::get_media_segment(const std::size_t segment_number) const
{
    const std::vector<dash::mpd::ISegmentURL *>& segment_urls = segment_list_->GetSegmentURLs();
    if (segment_urls.size() > segment_number)
    {
        return segment_urls.at(segment_number)->ToMediaSegment(base_urls_);
    }

    return nullptr;
}

dash::mpd::ISegment *segment_list_stream::get_bitstream_switching_segment() const
{
    if (const dash::mpd::IURLType *bitrstream_switching = segment_list_->GetBitstreamSwitching())
    {
        return bitrstream_switching->ToSegment(base_urls_);
    }

    return nullptr;
}

representation_stream::type segment_list_stream::get_stream_type() const
{
    return representation_stream::type::segment_list;
}

std::uint32_t segment_list_stream::get_size() const
{
    return static_cast<std::uint32_t>(segment_list_->GetSegmentURLs().size());
}

std::uint32_t segment_list_stream::get_average_segment_duration() const
{
    return segment_list_->GetDuration();
}

} // namespace ms::mpd
