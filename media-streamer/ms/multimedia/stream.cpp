#include <ms/multimedia/stream.hpp>

namespace ms::multimedia
{

stream::stream(const dash::mpd::IMPD &mpd, const std::size_t segment_buffer_size, const std::size_t frame_buffer_size,
               const std::size_t sample_buffer_size)
    : manager_(segment_buffer_size, *this, mpd),
      video_frames_(frame_buffer_size),
      audio_frames_(sample_buffer_size)
{}

bool stream::start()
{
    if (!manager_.start())
    {
        return false;
    }

    return true;
}

void stream::stop()
{
    manager_.stop();
}

std::optional<media::video_frame> stream::get_video_frame()
{
    return video_frames_.pop();
}

std::optional<media::audio_frame> stream::get_audio_frame()
{
    return audio_frames_.pop();
}

void stream::set_representation(const dash::mpd::IPeriod &period, const dash::mpd::IAdaptationSet &adaptation_set,
                                const dash::mpd::IRepresentation &representation)
{
    manager_.set_representation(period, adaptation_set, representation);
}

void stream::accept(const media::video_frame frame)
{
    video_frames_.push(frame);
}

void stream::accept(const media::audio_frame frame)
{
    audio_frames_.push(frame);
}

} // namespace ms::multimedia
