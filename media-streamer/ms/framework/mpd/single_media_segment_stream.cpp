#include <ms/framework/mpd/single_media_segment_stream.hpp>

#include <limits>

namespace ms::mpd
{

single_media_segment_stream::single_media_segment_stream(const dash::mpd::IMPD &mpd, const dash::mpd::IPeriod &period,
                                                         const dash::mpd::IAdaptationSet &adaptation_set,
                                                         const dash::mpd::IRepresentation &representation)
    : abstract_representation_stream(mpd, period, adaptation_set, representation)
{}

dash::mpd::ISegment *single_media_segment_stream::get_initialization_segment() const
{
    if (const dash::mpd::ISegmentBase *segment_base = representation_.GetSegmentBase())
    {
        if (const dash::mpd::IURLType *initialization = segment_base->GetInitialization())
        {
            return initialization->ToSegment(base_urls_);
        }
    }

    return nullptr;
}

dash::mpd::ISegment *single_media_segment_stream::get_index_segment(const std::size_t) const
{
    if (const dash::mpd::ISegmentBase *segment_base = representation_.GetSegmentBase())
    {
        if (const dash::mpd::IURLType *representation_index = segment_base->GetRepresentationIndex())
        {
            return representation_index->ToSegment(base_urls_);
        }
    }

    return nullptr;
}

dash::mpd::ISegment *single_media_segment_stream::get_media_segment(const std::size_t segment_number) const
{
    const std::vector<dash::mpd::IBaseUrl *> &base_urls = representation_.GetBaseURLs();
    if (base_urls.size() > segment_number)
    {
        return base_urls.at(segment_number)->ToMediaSegment(base_urls);
    }

    return base_urls.front()->ToMediaSegment(base_urls);
}

dash::mpd::ISegment *single_media_segment_stream::get_bitstream_switching_segment() const
{
    return nullptr;
}

representation_stream::type single_media_segment_stream::get_stream_type() const
{
    return representation_stream::type::single_media_segment;
}

std::uint32_t single_media_segment_stream::get_size() const
{
    return std::numeric_limits<std::uint32_t>::max() - 1;
}

std::uint32_t single_media_segment_stream::get_first_segment_number() const
{
    return 0;
}

std::uint32_t single_media_segment_stream::get_current_segment_number() const
{
    return 0;
}

std::uint32_t single_media_segment_stream::get_last_segment_number() const
{
    return 0;
}

std::uint32_t single_media_segment_stream::get_average_segment_duration() const
{
    return 1;
}

} // namespace ms::mpd
