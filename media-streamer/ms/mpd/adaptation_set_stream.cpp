#include <ms/mpd/adaptation_set_stream.hpp>

namespace ms::mpd
{

namespace
{

[[nodiscard]] std::shared_ptr<representation_stream> make_representation_stream(
        representation_stream::type, dash::mpd::IMPD &, dash::mpd::IPeriod &,
        dash::mpd::IAdaptationSet &, const dash::mpd::IRepresentation &)
{
    return nullptr;
}

[[nodiscard]] std::map<const dash::mpd::IRepresentation *, std::shared_ptr<representation_stream>> make_representations_map(
        dash::mpd::IMPD &mpd, dash::mpd::IAdaptationSet &adaptation_set, dash::mpd::IPeriod &period)
{
    const auto determine_representation_stream_type =
        [&adaptation_set, &period](const dash::mpd::IRepresentation &representation)
        {
            if (representation.GetSegmentList())
                return representation_stream::type::segment_list;

            if (representation.GetSegmentTemplate())
                return representation_stream::type::segment_template;

            if (representation.GetSegmentBase() || !representation.GetBaseURLs().empty())
                return representation_stream::type::single_media_segment;

            if (adaptation_set.GetSegmentList())
                return representation_stream::type::segment_list;

            if (adaptation_set.GetSegmentTemplate())
                return representation_stream::type::segment_template;

            if (adaptation_set.GetSegmentBase())
                return representation_stream::type::single_media_segment;

            if (period.GetSegmentList())
                return representation_stream::type::segment_list;

            if (period.GetSegmentTemplate())
                return representation_stream::type::segment_template;

            if (period.GetSegmentBase())
                return representation_stream::type::single_media_segment;

            return representation_stream::type::undefined;
        };

    std::map<const dash::mpd::IRepresentation *, std::shared_ptr<representation_stream>> representation_to_stream;
    const std::vector<dash::mpd::IRepresentation *> &representations = adaptation_set.GetRepresentation();

    for (const dash::mpd::IRepresentation *representation_pointer : representations)
    {
        const dash::mpd::IRepresentation &representation = *representation_pointer;

        const representation_stream::type type = determine_representation_stream_type(representation);
        representation_to_stream[representation_pointer] =
            make_representation_stream(type, mpd, period, adaptation_set, representation);
    }

    return representation_to_stream;
}

} // namespace

adaptation_set_stream::adaptation_set_stream(dash::mpd::IMPD &mpd, dash::mpd::IPeriod &period,
                                             dash::mpd::IAdaptationSet &adaptation_set)
    : representations_(make_representations_map(mpd, adaptation_set, period))
{}

} // namespace ms::mpd
