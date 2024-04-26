#include <ms/framework/stream/receiver.hpp>

namespace ms::framework::stream
{

receiver::receiver(const dash::mpd::IMPD &mpd, const std::uint32_t buffer_capacity)
    : mpd_(mpd),
      period_(mpd_.GetPeriods().front()),
      adaptation_set_(period_->GetAdaptationSets().front()),
      representation_(adaptation_set_->GetRepresentation().front()),
      adaptation_set_stream_(std::make_shared<mpd::adaptation_set_stream>(mpd_, *period_, *adaptation_set_)),
      representation_stream_(adaptation_set_stream_->get_representation_stream(*representation_)),
      segment_offset_(calculate_segment_offset()),
      buffer_(buffer_capacity)
{}

receiver::~receiver()
{
    stop();
}

bool receiver::start()
{
    if (is_buffering_.load())
    {
        return {};
    }

    is_buffering_.store(true);
    buffering_thread_ = std::thread([this] { do_buffering(); });
    return true;
}

void receiver::stop()
{
    if (!is_buffering_.load())
    {
        return;
    }

    is_buffering_.store(false);
    buffer_.set_eos();

    if (buffering_thread_.joinable())
    {
        buffering_thread_.join();
    }
}

std::shared_ptr<data::object> receiver::get_next_segment()
{
    if (segment_number_ >= representation_stream_->get_size())
    {
        return {};
    }

    if (const std::shared_ptr<dash::mpd::ISegment> segment =
            representation_stream_->get_media_segment(segment_number_ + segment_offset_))
    {
        ++segment_number_;
        return std::make_shared<data::object>(segment, *representation_);
    }

    return {};
}

std::shared_ptr<data::object> receiver::get_segment(const std::uint32_t segment_number)
{
    if (segment_number >= representation_stream_->get_size())
    {
        return {};
    }

    if (const std::shared_ptr<dash::mpd::ISegment> segment =
            representation_stream_->get_media_segment(segment_number + segment_offset_))
    {
        return std::make_shared<data::object>(segment, *representation_);
    }

    return {};
}

std::shared_ptr<data::object> receiver::get_initialization_segment()
{
    if (const std::shared_ptr<dash::mpd::ISegment> segment = representation_stream_->get_initialization_segment())
    {
        return std::make_shared<data::object>(segment, *representation_);
    }

    return {};
}

std::shared_ptr<data::object> receiver::find_initialization_segment(const dash::mpd::IRepresentation &representation)
{
    const auto representation_pointer = gsl::make_not_null(&representation);
    if (initialization_segments_.contains(representation_pointer))
    {
        return initialization_segments_[representation_pointer];
    }

    return {};
}

std::uint32_t receiver::get_position() const
{
    return segment_number_;
}

void receiver::set_position(const std::uint32_t segment_number)
{
    segment_number_ = segment_number;
}

void receiver::set_position_in_milliseconds(const std::uint32_t milliseconds)
{
    position_in_milliseconds_ = milliseconds;
}

void receiver::set_representation(const dash::mpd::IPeriod &period, const dash::mpd::IAdaptationSet &adaptation_set,
                                  const dash::mpd::IRepresentation &representation)
{
    const auto new_representation = gsl::make_not_null(&representation);
    if (representation_ == new_representation)
    {
        return;
    }

    representation_ = new_representation;

    const auto new_adaptation_set = gsl::make_not_null(&adaptation_set);
    if (adaptation_set_ != new_adaptation_set)
    {
        adaptation_set_ = new_adaptation_set;

        const auto new_period = gsl::make_not_null(&period);
        if (period_ != new_period)
        {
            period_ = new_period;
            segment_number_ = 0;
            segment_offset_ = calculate_segment_offset();
        }

        adaptation_set_stream_ = std::make_shared<mpd::adaptation_set_stream>(mpd_, period, adaptation_set);
    }

    representation_stream_ = adaptation_set_stream_->get_representation_stream(*representation_);
    download_initialization_segment(*representation_);
}

const dash::mpd::IRepresentation &receiver::get_representation() const
{
    return *representation_;
}

std::uint32_t receiver::calculate_segment_offset() const
{
    if (mpd_.GetType() == "static")
    {
        return {};
    }

    const std::uint32_t first_segment_number = representation_stream_->get_first_segment_number();
    const std::uint32_t current_segment_number = representation_stream_->get_current_segment_number();
    const std::uint32_t start_segment_number = current_segment_number - 2 * buffer_.capacity();

    return std::max(start_segment_number, first_segment_number);
}

void receiver::download_initialization_segment(const dash::mpd::IRepresentation &representation)
{
    const auto representation_pointer = gsl::make_not_null(&representation);
    if (initialization_segments_.contains(representation_pointer))
    {
        return;
    }

    if (const std::shared_ptr<data::object> initialization_segment = get_initialization_segment())
    {
        initialization_segment->start_download();
        initialization_segments_[representation_pointer] = initialization_segment;
    }
}

void receiver::do_buffering()
{
    download_initialization_segment(*representation_);
    std::shared_ptr<data::object> segment = get_next_segment();
    while (segment && is_buffering_.load())
    {
        segment->start_download();

        if (!buffer_.push(segment))
        {
            return;
        }

        segment->wait_finished();
        segment = get_next_segment();
    }

    buffer_.set_eos();
}

} // namespace ms::framework::stream
