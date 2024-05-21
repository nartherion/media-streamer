#pragma once

#include <ms/presentation/frame_renderer.hpp>
#include <ms/presentation/manager.hpp>
#include <ms/framework/stream/manager.hpp>

#include <string>
#include <memory>

#include <gsl/pointers>

#include <IMPD.h>
#include <IDASHManager.h>

namespace ms::media
{

class manager
{
public:
    struct configuration
    {
        struct stream
        {
            const dash::mpd::IAdaptationSet &adaptation_set_;
            const dash::mpd::IRepresentation &representation_;
        };

        const dash::mpd::IPeriod& period_;
        std::optional<stream> video_stream_;
        std::optional<stream> audio_stream_;
    };

    manager(presentation::frame_renderer &video_renderer, presentation::frame_renderer &audio_renderer);

    bool initialize(std::string url);
    bool start();
    void stop();
    std::shared_ptr<const dash::mpd::IMPD> get_mpd() const;
    void set_configuration(configuration c);

private:
    void start_video();
    void start_audio();
    void stop_video();
    void stop_audio();

    static void delete_dash_manager(dash::IDASHManager *dash_manager);

    constexpr static std::size_t segment_buffer_size = 4;
    constexpr static std::size_t frame_buffer_size = 4;
    constexpr static std::size_t sample_buffer_size = 4;

    std::unique_ptr<dash::IDASHManager, decltype(&delete_dash_manager)> dash_manager_;
    std::shared_ptr<const dash::mpd::IMPD> mpd_;
    std::optional<configuration> configuration_;

    presentation::manager video_presentation_manager_;
    std::optional<framework::stream::manager> video_stream_manager_;

    presentation::manager audio_presentation_manager_;
    std::optional<framework::stream::manager> audio_stream_manager_;

    bool is_started_ = false;
};

} // namespace ms::media
