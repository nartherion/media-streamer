#include <ms/framework/mpd/representation_stream_factory.hpp>
#include <ms/framework/mpd/segment_list_stream.hpp>
#include <ms/framework/mpd/segment_template_stream.hpp>
#include <ms/framework/mpd/single_media_segment_stream.hpp>

namespace ms::framework::mpd
{

namespace
{

std::vector<dash::mpd::IBaseUrl *> resolve_base_url(const std::shared_ptr<const dash::mpd::IMPD> mpd,
                                                    const dash::mpd::IPeriod &period,
                                                    const dash::mpd::IAdaptationSet &adaptation_set,
                                                    const std::size_t mpd_base_url_index,
                                                    const std::size_t period_base_url_index,
                                                    const std::size_t adaptation_set_base_url_index)
{
    std::vector<dash::mpd::IBaseUrl *> base_urls;

    const std::vector<dash::mpd::IBaseUrl *> &mpd_base_urls = mpd->GetBaseUrls();
    if (!mpd_base_urls.empty())
    {
        if (mpd_base_urls.size() > mpd_base_url_index)
        {
            base_urls.push_back(mpd_base_urls[mpd_base_url_index]);
        }
        else
        {
            base_urls.push_back(mpd_base_urls.front());
        }
    }

    const std::vector<dash::mpd::IBaseUrl *> &period_base_urls = period.GetBaseURLs();
    if (!period_base_urls.empty())
    {
        if (period_base_urls.size() > period_base_url_index)
        {
            base_urls.push_back(period_base_urls[period_base_url_index]);
        }
        else
        {
            base_urls.push_back(period_base_urls.front());
        }
    }

    const std::vector<dash::mpd::IBaseUrl *> &adaptation_set_base_urls = adaptation_set.GetBaseURLs();
    if (!adaptation_set_base_urls.empty())
    {
        if (adaptation_set_base_urls.size() > adaptation_set_base_url_index)
        {
            base_urls.push_back(adaptation_set_base_urls[adaptation_set_base_url_index]);
        }
        else
        {
            base_urls.push_back(adaptation_set_base_urls.front());
        }
    }

    if (!base_urls.empty())
    {
        const std::string &url = base_urls.front()->GetUrl();
        if (!url.starts_with("http://") && !url.starts_with("https://"))
        {
            base_urls.insert(base_urls.begin(), mpd->GetMPDPathBaseUrl());
        }
    }
    else
    {
        base_urls.push_back(mpd->GetMPDPathBaseUrl());
    }

    return base_urls;
}

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

std::shared_ptr<representation_stream> representation_stream_factory::create(
        const representation_stream::type type, const std::shared_ptr<const dash::mpd::IMPD> mpd,
        const dash::mpd::IPeriod &period, const dash::mpd::IAdaptationSet &adaptation_set,
        const dash::mpd::IRepresentation &representation)
{
    const std::vector<dash::mpd::IBaseUrl *> base_urls = resolve_base_url(mpd, period, adaptation_set, {}, {}, {});

    if (type == representation_stream::type::segment_list)
    {
        if (const dash::mpd::ISegmentList *segment_list = find_segment_list(period, adaptation_set, representation))
        {
            return std::make_shared<segment_list_stream>(mpd, base_urls, *segment_list);
        }

        return nullptr;
    }

    if (type == representation_stream::type::segment_template)
    {
        if (const dash::mpd::ISegmentTemplate *segment_template =
                find_segment_template(period, adaptation_set, representation) )
        {
            return std::make_shared<segment_template_stream>(mpd, base_urls, representation, *segment_template);
        }

        return nullptr;
    }

    if (type == representation_stream::type::single_media_segment)
    {
        return std::make_shared<single_media_segment_stream>(mpd, base_urls, representation);
    }

    return {};
}

} // namespace ms::framework::mpd
