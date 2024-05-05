#include <ms/media/manager.hpp>

#include <spdlog/spdlog.h>

#include <libdash.h>

namespace ms::media
{

manager::manager(presentation::frame_renderer &video_renderer, presentation::frame_renderer &audio_renderer)
    : dash_manager_(CreateDashManager(), delete_dash_manager),
      video_presentation_manager_(video_renderer, frame_buffer_size),
      audio_presentation_manager_(audio_renderer, sample_buffer_size)
{}

bool manager::initialize(std::string url)
{
    mpd_ = dash_manager_->Open(url.data());
    if (!mpd_)
    {
        SPDLOG_ERROR("Failed to parse the MPD: {}", url);
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

    if (!configuration_.has_value())
    {
        SPDLOG_ERROR("Failed to start stream: Configuration is missing");
        return false;
    }

    const auto &[period, video_stream, audio_stream] = configuration_.value();

    if (video_stream.has_value())
    {
        start_video();
    }
    else
    {
        SPDLOG_WARN("Video configuration is missing");
    }

    if (audio_stream.has_value())
    {
        start_audio();
    }
    else
    {
        SPDLOG_WARN("Audio configuration is missing");
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

const dash::mpd::IMPD *manager::get_mpd()
{
    return mpd_;
}

void manager::set_configuration(const configuration c)
{
    const auto &[period, video_stream, audio_stream] = configuration_.emplace(c);

    if (video_stream_manager_.has_value() && video_stream.has_value())
    {
        const auto &[adaptation_set, representation] = video_stream.value();
        video_stream_manager_->set_representation(period, adaptation_set, representation);
    }

    if (audio_stream_manager_.has_value() && audio_stream.has_value())
    {
        const auto &[adaptation_set, representation] = audio_stream.value();
        audio_stream_manager_->set_representation(period, adaptation_set, representation);
    }
}

void manager::start_video()
{
    if (!video_stream_manager_.has_value())
    {
        video_stream_manager_.emplace(segment_buffer_size, *mpd_, video_presentation_manager_);
    }

    if (configuration_.has_value())
    {
        const dash::mpd::IPeriod &period = configuration_->period_;

        if (const std::optional<configuration::stream> video_stream = configuration_->video_stream_)
        {
            const auto &[adaptation_set, representation] = video_stream.value();
            video_stream_manager_->set_representation(period, adaptation_set, representation);

            video_presentation_manager_.start();
            video_stream_manager_->start();
        }
    }
}

void manager::start_audio()
{
    if (!audio_stream_manager_.has_value())
    {
        audio_stream_manager_.emplace(segment_buffer_size, *mpd_, audio_presentation_manager_);
    }

    if (configuration_.has_value())
    {
        const dash::mpd::IPeriod &period = configuration_->period_;

        if (const std::optional<configuration::stream> audio_stream = configuration_->audio_stream_)
        {
            const auto &[adaptation_set, representation] = audio_stream.value();
            audio_stream_manager_->set_representation(period, adaptation_set, representation);

            audio_presentation_manager_.start();
            audio_stream_manager_->start();
        }
    }
}

void manager::stop_video()
{
    video_stream_manager_->stop();
    video_presentation_manager_.stop();
}

void manager::stop_audio()
{
    audio_stream_manager_->stop();
    audio_presentation_manager_.stop();
}

void manager::delete_dash_manager(dash::IDASHManager *dash_manager)
{
    dash_manager->Delete();
}

} // namespace ms::media
