#include <ms/framework/mpd/representation_stream_factory.hpp>
#include <ms/framework/mpd/segment_list_stream.hpp>
#include <ms/framework/mpd/segment_template_stream.hpp>
#include <ms/framework/mpd/single_media_segment_stream.hpp>

namespace ms::mpd
{

std::shared_ptr<representation_stream> representation_stream_factory::create(
        const representation_stream::type type, const dash::mpd::IMPD &mpd, const dash::mpd::IPeriod &period,
        const dash::mpd::IAdaptationSet &adaptation_set, const dash::mpd::IRepresentation &representation)
{
    switch (type)
    {
    case representation_stream::type::segment_list:
        return std::make_shared<segment_list_stream>(mpd, period, adaptation_set, representation);

    case representation_stream::type::segment_template:
        return std::make_shared<segment_template_stream>(mpd, period, adaptation_set, representation);

    case representation_stream::type::single_media_segment:
        return std::make_shared<single_media_segment_stream>(mpd, period, adaptation_set, representation);

    case representation_stream::type::undefined:
        return nullptr;
    }

    return nullptr;
}

} // namespace ms::mpd
