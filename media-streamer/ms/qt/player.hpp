#pragma once

#include <ms/media/manager.hpp>
#include <ms/qt/gl_renderer.hpp>
#include <ms/qt/audio_player.hpp>

#include <memory>

#include <QMainWindow>

#include <IMPD.h>

namespace Ui
{

class qt_player;

} // namespace Ui

namespace ms::qt
{

class player : public QMainWindow
{
    Q_OBJECT

public:
    player();

private slots:
    void on_period_combo_box_currentIndexChanged(int index);
    void on_video_as_combo_box_currentIndexChanged(int index);
    void on_video_representation_combo_box_currentIndexChanged(int index);
    void on_audio_as_combo_box_currentIndexChanged(int index);
    void on_audio_representation_combo_box_currentIndexChanged(int index);
    void on_download_mpd_button_clicked();
    void on_start_button_clicked();
    void on_stop_button_clicked();

private:
    class ui_mutex
    {
    public:
        ui_mutex(player &instance);

        void lock();
        void unlock();

    private:
        player &instance_;
    };

    std::string get_url();
    const dash::mpd::IPeriod *get_period(const dash::mpd::IMPD *mpd);
    const dash::mpd::IAdaptationSet *get_video_adaptation_set(const dash::mpd::IPeriod *period);
    const dash::mpd::IAdaptationSet *get_audio_adaptation_set(const dash::mpd::IPeriod *period);
    const dash::mpd::IRepresentation *get_video_representation(const dash::mpd::IAdaptationSet *adaptation_set);
    const dash::mpd::IRepresentation *get_audio_representation(const dash::mpd::IAdaptationSet *adaptation_set);
    std::optional<media::manager::configuration> make_configuration(const dash::mpd::IMPD *mpd);
    void update_configuration(const dash::mpd::IMPD *mpd);
    std::shared_ptr<Ui::qt_player> make_gui();
    void update_gui(const dash::mpd::IMPD *mpd = nullptr);
    void set_control_buttons_enabled(bool enabled);
    void on_period_changed(const dash::mpd::IPeriod *period);
    void configure_period_combo_box(const std::vector<dash::mpd::IPeriod *> &periods);
    void configure_adaptation_set_combo_boxes(
        const std::vector<const dash::mpd::IAdaptationSet *> &video_adaptation_sets,
        const std::vector<const dash::mpd::IAdaptationSet *> &audio_adaptation_sets);
    void configure_video_representation_combo_box(const dash::mpd::IAdaptationSet *video_adaptation_set);
    void configure_audio_representation_combo_box(const dash::mpd::IAdaptationSet *audio_adaptation_set);

    const std::shared_ptr<Ui::qt_player> ui_;
    ui_mutex ui_mutex_;
    gl_renderer *gl_renderer_;
    audio_player audio_player_;
    media::manager media_manager_;
};

} // namespace ms::qt
