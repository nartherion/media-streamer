#include <ms/framework/mpd/abstract_representation_stream.hpp>
#include <ms/framework/mpd/time_utils.hpp>

#include <chrono>

namespace ms::mpd
{

namespace
{

std::vector<dash::mpd::IBaseUrl *> resolve_base_url(const dash::mpd::IMPD &mpd, const dash::mpd::IPeriod &period,
                                                    const dash::mpd::IAdaptationSet &adaptation_set,
                                                    const std::size_t mpd_base_url, std::size_t period_base_url,
                                                    const std::size_t adaptation_set_base_url)
{
    std::vector<dash::mpd::IBaseUrl *> base_urls;

    const std::vector<dash::mpd::IBaseUrl *> &mpd_base_urls = mpd.GetBaseUrls();
    const std::vector<dash::mpd::IBaseUrl *> &period_base_urls = period.GetBaseURLs();
    const std::vector<dash::mpd::IBaseUrl *> &adaptation_set_base_urls = adaptation_set.GetBaseURLs();

    if (!mpd_base_urls.empty())
    {
        if (mpd_base_urls.size() > mpd_base_url)
        {
            base_urls.push_back(mpd_base_urls[mpd_base_url]);
        }
        else
        {
            base_urls.push_back(mpd_base_urls.front());
        }
    }

    if (!period_base_urls.empty())
    {
        if (period_base_urls.size() > mpd_base_url)
        {
            base_urls.push_back(period_base_urls[period_base_url]);
        }
        else
        {
            base_urls.push_back(period_base_urls.front());
        }
    }

    if (!adaptation_set_base_urls.empty())
    {
        if (adaptation_set_base_urls.size() > mpd_base_url)
        {
            base_urls.push_back(adaptation_set_base_urls[adaptation_set_base_url]);
        }
        else
        {
            base_urls.push_back(adaptation_set_base_urls.front());
        }
    }

    if (!base_urls.empty())
    {
        const std::string &url = base_urls.front()->GetUrl();

        if (url.substr(0, 7) != "http://" && url.substr(0, 8) != "https://")
        {
            base_urls.insert(base_urls.begin(), mpd.GetMPDPathBaseUrl());
        }
    }
    else
    {
        base_urls.push_back(mpd.GetMPDPathBaseUrl());
    }

    return base_urls;
}

} // namespace

abstract_representation_stream::abstract_representation_stream(const dash::mpd::IMPD &mpd,
                                                               const dash::mpd::IPeriod &period,
                                                               const dash::mpd::IAdaptationSet &adaptation_set,
                                                               const dash::mpd::IRepresentation &representation)
    : base_urls_(resolve_base_url(mpd, period, adaptation_set, 0, 0, 0)),
      mpd_(mpd),
      period_(period),
      adaptation_set_(adaptation_set),
      representation_(representation)
{}

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
