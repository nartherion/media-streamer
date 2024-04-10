#include <ms/mpd/abstract_representation_stream.hpp>
#include <ms/mpd/time_utils.hpp>

#include <chrono>

namespace ms::mpd
{

abstract_representation_stream::abstract_representation_stream(dash::mpd::IMPD &mpd, dash::mpd::IPeriod &period,
                                                               dash::mpd::IAdaptationSet &adaptation_set,
                                                               dash::mpd::IRepresentation &representation)
    : mpd_(mpd),
      period_(period),
      adaptation_set_(adaptation_set),
      representation_(representation)
{}

void abstract_representation_stream::set_base_urls(std::vector<dash::mpd::IBaseUrl *> base_urls)
{
    base_urls_ = std::move(base_urls);
}

std::uint32_t abstract_representation_stream::get_first_segment_number() const
{
    if (mpd_.GetType() == "dynamic")
    {
        const std::uint32_t current_time = get_current_time_in_seconds();
        const std::uint32_t availability_start_time = get_utc_time_in_seconds(mpd_.GetAvailabilityStarttime());
        const std::uint32_t segment_duration = get_average_segment_duration();
        const std::uint32_t timeshift = get_duration_in_seconds(mpd_.GetTimeShiftBufferDepth());

        return (current_time - availability_start_time - segment_duration - timeshift) / segment_duration;
    }

    return {};
}

std::uint32_t abstract_representation_stream::get_current_segment_number() const
{
    if (mpd_.GetType() == "dynamic")
    {
        const std::uint32_t current_time = get_current_time_in_seconds();
        const std::uint32_t segment_duration = get_average_segment_duration();
        const std::uint32_t availability_start_time = get_utc_time_in_seconds(mpd_.GetAvailabilityStarttime());

        return (current_time - segment_duration - availability_start_time) / segment_duration;
    }

    return {};
}

std::uint32_t abstract_representation_stream::get_last_segment_number() const
{
    if (mpd_.GetType() == "dynamic")
    {
        const std::uint32_t current_time = get_current_time_in_seconds();
        const std::uint32_t segment_duration = get_average_segment_duration();
        const std::uint32_t availability_start_time = get_utc_time_in_seconds(mpd_.GetAvailabilityStarttime());
        const std::uint32_t check_time = mpd_.GetFetchTime() + get_duration_in_seconds(mpd_.GetMinimumUpdatePeriod());
        const std::uint32_t last_time = std::max(check_time, current_time);

        return (last_time - segment_duration - availability_start_time) / segment_duration;
    }

    return {};
}

} // namespace ms::mpd
