#include <ms/framework/stream/receiver.hpp>

#include <spdlog/spdlog.h>

namespace ms::framework::stream
{

receiver::receiver(const dash::mpd::IMPD &mpd, const std::size_t buffer_size)
    : mpd_(mpd),
      buffer_size_(buffer_size),
      period_(mpd_.GetPeriods().front()),
      adaptation_set_(period_->GetAdaptationSets().front()),
      representation_(adaptation_set_->GetRepresentation().front()),
      adaptation_set_stream_(std::make_optional<mpd::adaptation_set_stream>(mpd_, *period_, *adaptation_set_)),
      representation_stream_(adaptation_set_stream_->get_representation_stream(*representation_)),
      segment_offset_(calculate_segment_offset())
{}

receiver::~receiver()
{
    stop();
}

bool receiver::start()
{
    if (is_buffering_.load())
    {
        return false;
    }

    buffer_.emplace(buffer_size_);
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

    buffer_->set_eos();
    is_buffering_.store(false);
    if (buffering_thread_.joinable())
    {
        buffering_thread_.join();
    }
}

void receiver::set_representation(const dash::mpd::IPeriod &period, const dash::mpd::IAdaptationSet &adaptation_set,
                                  const dash::mpd::IRepresentation &representation)
{
    std::scoped_lock lock(monitor_mutex_);

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

        adaptation_set_stream_.emplace(mpd_, period, adaptation_set);
    }

    representation_stream_ = adaptation_set_stream_->get_representation_stream(*representation_);

    settings_updated_.store(true);
}

std::shared_ptr<data::object> receiver::get_oldest_segment()
{
    if (is_buffering_.load())
    {
        return buffer_->pop();
    }

    return {};
}

std::shared_ptr<data::object> receiver::find_initialization_segment(
        const dash::mpd::IRepresentation &representation) const
{
    std::scoped_lock lock(monitor_mutex_);

    const auto rp = gsl::make_not_null(&representation);
    if (initialization_segments_.contains(rp))
    {
        return initialization_segments_.at(rp);
    }

    return {};
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

std::shared_ptr<data::object> receiver::get_initialization_segment()
{
    if (const std::shared_ptr<dash::mpd::ISegment> segment = representation_stream_->get_initialization_segment())
    {
        return std::make_shared<data::object>(segment, *representation_);
    }

    return {};
}

std::uint32_t receiver::calculate_segment_offset() const
{
    if (mpd_.GetType() == "static")
    {
        return {};
    }

    const std::uint32_t first_segment_number = representation_stream_->get_first_segment_number();
    const std::uint32_t current_segment_number = representation_stream_->get_current_segment_number();
    const auto start_segment_number = static_cast<std::uint32_t>(current_segment_number - 2 * buffer_size_);

    return std::max(start_segment_number, first_segment_number);
}

void receiver::download_initialization_segment()
{
    std::scoped_lock lock(monitor_mutex_);

    if (initialization_segments_.contains(representation_))
    {
        return;
    }

    if (const std::shared_ptr<data::object> initialization_segment = get_initialization_segment())
    {
        initialization_segment->start_download();
        initialization_segment->wait_finished();

        initialization_segments_[representation_] = initialization_segment;
    }
}

void receiver::do_buffering()
{
    while (const std::shared_ptr<data::object> segment = get_next_segment())
    {
        if (!is_buffering_.load())
        {
            break;
        }

        segment->start_download();
        segment->wait_finished();

        if (!buffer_->push(segment))
        {
            break;
        }

        if (settings_updated_.load())
        {
            settings_updated_.store(false);
            download_initialization_segment();
        }
    }

    buffer_->set_eos();
}

} // namespace ms::framework::stream
