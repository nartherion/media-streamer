#pragma once

#include <ms/multimedia/video_renderer.hpp>
#include <ms/multimedia/audio_player.hpp>
#include <ms/multimedia/stream.hpp>

#include <string>
#include <memory>

#include <gsl/pointers>

#include <IMPD.h>
#include <IDASHManager.h>

namespace ms::multimedia
{

class manager
{
public:
    manager(video_renderer &renderer, audio_player &player);

    bool initialize(std::string url);
    bool start();
    void stop();
    const dash::mpd::IMPD &get_mpd();
    void set_video_quality(const dash::mpd::IPeriod &period, const dash::mpd::IAdaptationSet &adaptation_set,
                           const dash::mpd::IRepresentation &representation);
    void set_audio_quality(const dash::mpd::IPeriod &period, const dash::mpd::IAdaptationSet &adaptation_set,
                           const dash::mpd::IRepresentation &representation);
    void set_frame_rate(int frame_rate);

private:
    void start_video();
    void start_audio();
    void stop_video();
    void stop_audio();
    void do_render_video();
    void do_play_audio();

    static void delete_dash_manager(dash::IDASHManager *dash_manager);

    constexpr static std::size_t segment_buffer_size = 4;
    constexpr static std::size_t frame_buffer_size = 4;
    constexpr static std::size_t sample_buffer_size = 4;

    std::unique_ptr<dash::IDASHManager, decltype(&delete_dash_manager)> dash_manager_;
    video_renderer &renderer_;
    audio_player &player_;
    const dash::mpd::IMPD* mpd_ = nullptr;
    const dash::mpd::IPeriod* period_ = nullptr;
    std::optional<stream> video_stream_;
    const dash::mpd::IAdaptationSet *video_adaptation_set_ = nullptr;
    const dash::mpd::IRepresentation *video_representation_ = nullptr;
    int frame_rate = 0;
    std::thread video_renderer_thread_;
    std::atomic<bool> is_video_rendering_ = false;
    std::optional<stream> audio_stream_;
    const dash::mpd::IAdaptationSet *audio_adaptation_set_ = nullptr;
    const dash::mpd::IRepresentation *audio_representation_ = nullptr;
    std::thread audio_player_thread_;
    std::atomic<bool> is_audio_playing_ = false;
    bool is_started_ = false;
};

} // namespace ms::multimedia
