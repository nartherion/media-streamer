#include <ms/framework/mpd/segment_template_stream.hpp>
#include <ms/framework/mpd/time_utils.hpp>

#include <cmath>

namespace ms::framework::mpd
{

namespace
{

std::vector<std::uint32_t> calculate_segment_start_times(const dash::mpd::ISegmentTemplate &segment_template)
{
    const dash::mpd::ISegmentTimeline *segment_timeline = segment_template.GetSegmentTimeline();
    if (!segment_timeline)
        return {};

    std::vector<std::uint32_t> segment_start_times;

    for (const dash::mpd::ITimeline *timeline : segment_timeline->GetTimelines())
    {
        const std::uint32_t segment_start_time = timeline->GetStartTime();
        const std::uint32_t segment_duration = timeline->GetDuration();
        const std::uint32_t repeat_count = timeline->GetRepeatCount();

        if (repeat_count > 0)
        {
            for (std::uint32_t index = 0; index <= repeat_count; ++index)
            {
                if (segment_start_time > 0)
                {
                    segment_start_times.push_back(segment_start_time + segment_duration * index);
                }
            }
        }
        else
        {
            segment_start_times.push_back(segment_start_time);
        }
    }

    return segment_start_times;
}

} // namespace

segment_template_stream::segment_template_stream(const dash::mpd::IMPD &mpd,
                                                 const std::vector<dash::mpd::IBaseUrl *> &base_urls,
                                                 const dash::mpd::IRepresentation &representation,
                                                 const dash::mpd::ISegmentTemplate &segment_template)
    : abstract_representation_stream(mpd, base_urls),
      representation_(representation),
      segment_template_(segment_template),
      segment_start_times_(calculate_segment_start_times(segment_template))
{}

std::shared_ptr<dash::mpd::ISegment> segment_template_stream::get_initialization_segment() const
{
    if (const dash::mpd::IURLType *initialization = segment_template_.GetInitialization())
    {
        return std::shared_ptr<dash::mpd::ISegment>(initialization->ToSegment(base_urls_));
    }

    return std::shared_ptr<dash::mpd::ISegment>(
        segment_template_.ToInitializationSegment(base_urls_, representation_.GetId(), representation_.GetBandwidth()));
}

std::shared_ptr<dash::mpd::ISegment> segment_template_stream::get_index_segment(const std::size_t segment_number) const
{
    if (segment_template_.GetSegmentTimeline())
    {
        if (segment_start_times_.size() > segment_number)
        {
            return std::shared_ptr<dash::mpd::ISegment>(
                segment_template_.GetIndexSegmentFromTime(base_urls_, representation_.GetId(),
                                                          representation_.GetBandwidth(),
                                                          segment_start_times_[segment_number]));
        }

        return {};
    }

    const auto index_segment_number = segment_template_.GetStartNumber() + static_cast<std::uint32_t>(segment_number);
    return std::shared_ptr<dash::mpd::ISegment>(
        segment_template_.GetIndexSegmentFromNumber(base_urls_, representation_.GetId(), representation_.GetBandwidth(),
                                                    index_segment_number));
}

std::shared_ptr<dash::mpd::ISegment> segment_template_stream::get_media_segment(const std::size_t segment_number) const
{
    if (segment_template_.GetSegmentTimeline())
    {
        if (segment_start_times_.size() > segment_number)
        {
            return std::shared_ptr<dash::mpd::ISegment>(
                segment_template_.GetMediaSegmentFromTime(base_urls_, representation_.GetId(),
                                                          representation_.GetBandwidth(),
                                                          segment_start_times_[segment_number]));
        }

        return {};
    }

    const auto media_segment_number = segment_template_.GetStartNumber() + static_cast<std::uint32_t>(segment_number);
    return std::shared_ptr<dash::mpd::ISegment>(
        segment_template_.GetMediaSegmentFromNumber(base_urls_, representation_.GetId(),
                                                    representation_.GetBandwidth(), media_segment_number));
}

std::shared_ptr<dash::mpd::ISegment> segment_template_stream::get_bitstream_switching_segment() const
{
    if (const dash::mpd::IURLType *bitrstream_switching = segment_template_.GetBitstreamSwitching())
    {
        return std::shared_ptr<dash::mpd::ISegment>(bitrstream_switching->ToSegment(base_urls_));
    }

    return std::shared_ptr<dash::mpd::ISegment>(
        segment_template_.ToBitstreamSwitchingSegment(base_urls_, representation_.GetId(),
                                                      representation_.GetBandwidth()));
}

representation_stream::type segment_template_stream::get_stream_type() const
{
    return representation_stream::type::segment_template;
}

std::uint32_t segment_template_stream::get_size() const
{
    if (!segment_start_times_.empty())
    {
        return static_cast<std::uint32_t>(segment_start_times_.size());
    }

    if (mpd_.GetType() != "static")
    {
        return std::numeric_limits<std::uint32_t>::max() - 1;
    }

    const auto media_presentation_duration =
        static_cast<double>(get_duration_in_seconds(mpd_.GetMediaPresentationDuration()));
    const auto number_of_segments = static_cast<std::uint32_t>(
        std::ceil(segment_template_.GetTimescale() * media_presentation_duration / segment_template_.GetDuration()));

    return number_of_segments;
}

std::uint32_t segment_template_stream::get_average_segment_duration() const
{
    return segment_template_.GetDuration();
}

} // namespace ms::framework::mpd
