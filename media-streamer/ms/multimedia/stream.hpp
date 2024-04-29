#pragma once

#include <ms/media/frame_acceptor.hpp>
#include <ms/framework/stream/manager.hpp>
#include <ms/common/buffer.hpp>

#include <cstddef>

#include <IMPD.h>

namespace ms::multimedia
{

class stream : public media::frame_acceptor
{
public:
    stream(const dash::mpd::IMPD &mpd, std::size_t segment_buffer_size, std::size_t frame_buffer_size,
           std::size_t sample_buffer_size);

    bool start();
    void stop();
    std::optional<av::frame> get_video_frame();
    std::optional<av::frame> get_audio_frame();
    void set_representation(const dash::mpd::IPeriod &period, const dash::mpd::IAdaptationSet &adaptation_set,
                            const dash::mpd::IRepresentation &representation);

private:
    void accept_video(av::frame frame) override;
    void accept_audio(av::frame frame) override;

    framework::stream::manager manager_;
    common::buffer<std::optional<av::frame>> video_frames_;
    common::buffer<std::optional<av::frame>> audio_frames_;
};

} // namespace ms::multimedia
