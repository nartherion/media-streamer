#include <ms/framework/mpd/segment_template_stream.hpp>
#include <ms/framework/mpd/time_utils.hpp>

#include <cmath>

namespace ms::mpd
{

namespace
{

dash::mpd::ISegmentTemplate *find_segment_template(
        const dash::mpd::IPeriod &period, const dash::mpd::IAdaptationSet &adaptation_set,
        const dash::mpd::IRepresentation &representation)
{
    if (dash::mpd::ISegmentTemplate *segment_template = representation.GetSegmentTemplate())
    {
        return segment_template;
    }

    if (dash::mpd::ISegmentTemplate *segment_template = adaptation_set.GetSegmentTemplate())
    {
        return segment_template;
    }

    if (dash::mpd::ISegmentTemplate *segment_template = period.GetSegmentTemplate())
    {
        return segment_template;
    }

    return nullptr;
}

} // namespace

segment_template_stream::segment_template_stream(const dash::mpd::IMPD &mpd, const dash::mpd::IPeriod &period,
                                                 const dash::mpd::IAdaptationSet &adaptation_set,
                                                 const dash::mpd::IRepresentation &representation)
    : abstract_representation_stream(mpd, period, adaptation_set, representation),
      segment_template_(find_segment_template(period, adaptation_set, representation))
{
    calculate_segment_start_times();
}

dash::mpd::ISegment *segment_template_stream::get_initialization_segment() const
{
    if (const dash::mpd::IURLType *initialization = segment_template_->GetInitialization())
    {
        return initialization->ToSegment(base_urls_);
    }

    return segment_template_->ToInitializationSegment(base_urls_, representation_.GetId(),
                                                      representation_.GetBandwidth());
}

dash::mpd::ISegment *segment_template_stream::get_index_segment(const std::size_t segment_number) const
{
    if (segment_template_->GetSegmentTimeline())
    {
        if (!segment_start_times_.empty())
        {
            return segment_template_->GetIndexSegmentFromTime(base_urls_, representation_.GetId(),
                                                              representation_.GetBandwidth(),
                                                              segment_start_times_.at(segment_number));
        }

        return nullptr;
    }

    return segment_template_->GetIndexSegmentFromNumber(base_urls_, representation_.GetId(),
                                                        representation_.GetBandwidth(),
                                                        segment_template_->GetStartNumber() +
                                                        static_cast<std::uint32_t>(segment_number));
}

dash::mpd::ISegment *segment_template_stream::get_media_segment(const std::size_t segment_number) const
{
    if (segment_template_->GetSegmentTimeline())
    {
        if (!segment_start_times_.empty())
        {
            return segment_template_->GetMediaSegmentFromTime(base_urls_, representation_.GetId(),
                                                              representation_.GetBandwidth(),
                                                              segment_start_times_.at(segment_number));
        }

        return nullptr;
    }

    return segment_template_->GetMediaSegmentFromNumber(base_urls_, representation_.GetId(),
                                                        representation_.GetBandwidth(),
                                                        segment_template_->GetStartNumber() +
                                                        static_cast<std::uint32_t>(segment_number));
}

dash::mpd::ISegment *segment_template_stream::get_bitstream_switching_segment() const
{
    if (const dash::mpd::IURLType *bitrstream_switching = segment_template_->GetBitstreamSwitching())
    {
        return bitrstream_switching->ToSegment(base_urls_);
    }

    return segment_template_->ToBitstreamSwitchingSegment(base_urls_, representation_.GetId(),
                                                          representation_.GetBandwidth());
}

representation_stream::type segment_template_stream::get_stream_type() const
{
    return representation_stream::type::segment_template;
}

std::uint32_t segment_template_stream::get_size() const
{
    if (!segment_start_times_.empty())
        return static_cast<std::uint32_t>(segment_start_times_.size());

    if (mpd_.GetType() != "static")
    {
        return std::numeric_limits<std::uint32_t>::max() - 1;
    }

    const auto media_presentation_duration =
        static_cast<double>(get_duration_in_seconds(mpd_.GetMediaPresentationDuration()));
    const auto number_of_segments = static_cast<std::uint32_t>(
        std::ceil(segment_template_->GetTimescale() * media_presentation_duration / segment_template_->GetDuration()));

    return number_of_segments;
}

std::uint32_t segment_template_stream::get_average_segment_duration() const
{
    return segment_template_->GetDuration();
}

void segment_template_stream::calculate_segment_start_times()
{
    const dash::mpd::ISegmentTimeline *segment_timeline = segment_template_->GetSegmentTimeline();
    if (!segment_timeline)
        return;

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
                    segment_start_times_.push_back(segment_start_time + segment_duration * index);
                }
            }
        }
        else
        {
            segment_start_times_.push_back(segment_start_time);
        }
    }
}

} // namespace ms::mpd
