#include <ms/multimedia/manager.hpp>

#include <spdlog/spdlog.h>

#include <libdash.h>

namespace ms::multimedia
{

manager::manager(video_renderer &renderer, audio_player &player)
    : dash_manager_(CreateDashManager(), delete_dash_manager),
      renderer_(renderer),
      player_(player)
{}

bool manager::initialize(std::string url)
{
    mpd_ = dash_manager_->Open(url.data());
    if (!mpd_)
    {
        SPDLOG_ERROR("Failed to parse the MPD");
        return false;
    }

    return true;
}

bool manager::start()
{
    if (is_started_)
    {
        return false;
    }

    if (!mpd_)
    {
        SPDLOG_ERROR("Failed to start stream: MPD is missing");
        return false;
    }

    if (!period_)
    {
        SPDLOG_ERROR("Failed to start stream: Period is missing");
        return false;
    }

    if (video_adaptation_set_ && video_representation_)
    {
        start_video();
    }
    else
    {
        SPDLOG_WARN("Video settings missing in MPD");
    }

    if (audio_adaptation_set_ && audio_representation_)
    {
        start_audio();
    }
    else
    {
        SPDLOG_WARN("Audio settings missing in MPD");
    }

    is_started_ = true;
    return true;
}

void manager::stop()
{
    if (!is_started_)
    {
        return;
    }

    stop_video();
    stop_audio();
    is_started_ = false;
    return;
}

void manager::set_video_quality(const dash::mpd::IPeriod &period, const dash::mpd::IAdaptationSet &adaptation_set,
                                const dash::mpd::IRepresentation &representation)
{
    period_ = &period;
    video_adaptation_set_ = &adaptation_set;
    video_representation_ = &representation;

    if (video_stream_.has_value())
    {
        video_stream_->set_representation(*period_, *video_adaptation_set_, *video_representation_);
    }
}

void manager::set_audio_quality(const dash::mpd::IPeriod &period, const dash::mpd::IAdaptationSet &adaptation_set,
                                const dash::mpd::IRepresentation &representation)
{
    period_ = &period;
    audio_adaptation_set_ = &adaptation_set;
    audio_representation_ = &representation;

    if (audio_stream_.has_value())
    {
        audio_stream_->set_representation(*period_, *audio_adaptation_set_, *audio_representation_);
    }
}

void manager::start_video()
{
    if (is_video_rendering_.load())
    {
        return;
    }

    assert(!video_stream_.has_value());
    assert(mpd_ && period_ && video_adaptation_set_ && video_representation_);

    video_stream_.emplace(*mpd_, segment_buffer_size, frame_buffer_size, 0);
    video_stream_->set_representation(*period_, *video_adaptation_set_, *video_representation_);
    video_stream_->start();

    is_video_rendering_.store(true);
    video_renderer_thread_ = std::thread([this] { do_render_video(); });
}

void manager::start_audio()
{
    if (is_audio_playing_.load())
    {
        return;
    }

    assert(!audio_stream_.has_value());
    assert(mpd_ && period_ && audio_adaptation_set_ && audio_representation_);

    audio_stream_.emplace(*mpd_, segment_buffer_size, 0, sample_buffer_size);
    audio_stream_->set_representation(*period_, *audio_adaptation_set_, *audio_representation_);
    audio_stream_->start();

    is_audio_playing_.store(true);
    audio_player_thread_ = std::thread([this] { do_play_audio(); });
}

void manager::stop_video()
{
    if (!is_video_rendering_.load())
    {
        return;
    }

    assert(video_stream_.has_value());
    video_stream_->stop();

    is_video_rendering_.store(false);
    if (video_renderer_thread_.joinable())
    {
        video_renderer_thread_.join();
    }
}

void manager::stop_audio()
{
    if (!is_audio_playing_.load())
    {
        return;
    }

    assert(audio_stream_.has_value());
    audio_stream_->stop();

    is_audio_playing_.store(false);
    if (audio_player_thread_.joinable())
    {
        audio_player_thread_.join();
    }
}

void manager::do_render_video()
{
    while (is_video_rendering_.load())
    {
        if (const std::optional<media::video_frame> frame = video_stream_->get_video_frame())
        {
            renderer_.render(frame.value());
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / frame_rate));
        }
    }
}

void manager::do_play_audio()
{
    while (is_audio_playing_.load())
    {
        if (const std::optional<media::audio_frame> frame = audio_stream_->get_audio_frame())
        {
            player_.play(frame.value());
        }
    }
}

} // namespace ms::multimedia
