#include <ms/framework/mpd/adaptation_set_stream.hpp>
#include <ms/framework/mpd/representation_stream_factory.hpp>

#include <optional>

namespace ms::framework::mpd
{

namespace
{

adaptation_set_stream::representation_stream_table make_representation_stream_table(
        const std::shared_ptr<const dash::mpd::IMPD> mpd, const dash::mpd::IAdaptationSet &adaptation_set,
        const dash::mpd::IPeriod &period)
{
    const auto get_representation_stream_type =
        [&adaptation_set, &period]
        (const dash::mpd::IRepresentation &representation) -> std::optional<representation_stream::type>
        {
            if (representation.GetSegmentList())
            {
                return representation_stream::type::segment_list;
            }

            if (representation.GetSegmentTemplate())
            {
                return representation_stream::type::segment_template;
            }

            if (representation.GetSegmentBase() || !representation.GetBaseURLs().empty())
            {
                return representation_stream::type::single_media_segment;
            }

            if (adaptation_set.GetSegmentList())
            {
                return representation_stream::type::segment_list;
            }

            if (adaptation_set.GetSegmentTemplate())
            {
                return representation_stream::type::segment_template;
            }

            if (adaptation_set.GetSegmentBase())
            {
                return representation_stream::type::single_media_segment;
            }

            if (period.GetSegmentList())
            {
                return representation_stream::type::segment_list;
            }

            if (period.GetSegmentTemplate())
            {
                return representation_stream::type::segment_template;
            }

            if (period.GetSegmentBase())
            {
                return representation_stream::type::single_media_segment;
            }

            return {};
        };

    adaptation_set_stream::representation_stream_table representation_stream_table;
    const std::vector<dash::mpd::IRepresentation *> representations = adaptation_set.GetRepresentation();
    for (const gsl::not_null<const dash::mpd::IRepresentation *> representation_pointer : representations)
    {
        const dash::mpd::IRepresentation &representation = *representation_pointer;
        if (const std::optional<representation_stream::type> type = get_representation_stream_type(representation))
        {
            representation_stream_table[representation_pointer] =
                representation_stream_factory::create(type.value(), mpd, period, adaptation_set, representation);
        }
    }

    return representation_stream_table;
}

} // namespace

adaptation_set_stream::adaptation_set_stream(std::shared_ptr<const dash::mpd::IMPD> mpd, const dash::mpd::IPeriod &period,
                                             const dash::mpd::IAdaptationSet &adaptation_set)
    : representation_stream_table_(make_representation_stream_table(mpd, adaptation_set, period))
{}

std::shared_ptr<representation_stream> adaptation_set_stream::get_representation_stream(
        const dash::mpd::IRepresentation &representation)
{
    const dash::mpd::IRepresentation *representation_pointer = &representation;

    if (representation_stream_table_.contains(representation_pointer))
        return representation_stream_table_[representation_pointer];

    return {};
}

} // namespace ms::framework::mpd
