#include <ms/qt/player.hpp>

#include "ui_qt_player.h"

#include <spdlog/spdlog.h>
#include <fmt/format.h>

namespace ms::qt
{

namespace
{

bool mime_type_contains(const dash::mpd::IAdaptationSet &adaptation_set, const std::string &value)
{
    const std::string adaptation_set_mime_type = adaptation_set.GetMimeType();
    if (adaptation_set_mime_type.find(value) != std::string::npos)
    {
        return true;
    }

    for (const dash::mpd::IRepresentation *representation : adaptation_set.GetRepresentation())
    {
        const std::string representation_mime_type = representation->GetMimeType();
        if (representation_mime_type.find(value) != std::string::npos)
        {
            return true;
        }
    }

    return false;
}

bool is_video_adaptation_set(const dash::mpd::IAdaptationSet &adaptation_set)
{
    return mime_type_contains(adaptation_set, "video");
}

bool is_audio_adaptation_set(const dash::mpd::IAdaptationSet &adaptation_set)
{
    return mime_type_contains(adaptation_set, "audio");
}

std::vector<const dash::mpd::IAdaptationSet *> get_video_adaptation_sets(const dash::mpd::IPeriod *period)
{
    if (!period)
    {
        return {};
    }

    std::vector<const dash::mpd::IAdaptationSet *> adaptation_sets;

    for (const dash::mpd::IAdaptationSet *adaptation_set : period->GetAdaptationSets())
    {
        if (is_video_adaptation_set(*adaptation_set))
        {
            adaptation_sets.push_back(adaptation_set);
        }
    }

    return adaptation_sets;
}

std::vector<const dash::mpd::IAdaptationSet *> get_audio_adaptation_sets(const dash::mpd::IPeriod *period)
{
    if (!period)
    {
        return {};
    }

    std::vector<const dash::mpd::IAdaptationSet *> adaptation_sets;

    for (const dash::mpd::IAdaptationSet *adaptation_set : period->GetAdaptationSets())
    {
        if (is_audio_adaptation_set(*adaptation_set))
        {
            adaptation_sets.push_back(adaptation_set);
        }
    }

    return adaptation_sets;
}

std::optional<media::manager::configuration::stream> make_stream_configuration(
        const dash::mpd::IAdaptationSet *adaptation_set, const dash::mpd::IRepresentation *representation)
{
    if (adaptation_set && representation)
    {
        return media::manager::configuration::stream
        {
            .adaptation_set_ = *adaptation_set,
            .representation_ = *representation
        };
    }

    return {};
}

} // namespace

void player::on_period_combo_box_currentIndexChanged([[maybe_unused]] const int index)
{
    const std::shared_ptr<const dash::mpd::IMPD> mpd = media_manager_.get_mpd();
    assert(mpd != nullptr);

    on_period_changed(get_period(mpd));
    update_configuration(mpd);
}

void player::on_video_as_combo_box_currentIndexChanged([[maybe_unused]] const int index)
{
    const std::shared_ptr<const dash::mpd::IMPD> mpd = media_manager_.get_mpd();
    assert(mpd != nullptr);

    configure_video_representation_combo_box(get_video_adaptation_set(get_period(mpd)));
    update_configuration(mpd);
}

void player::on_audio_as_combo_box_currentIndexChanged([[maybe_unused]] const int index)
{
    const std::shared_ptr<const dash::mpd::IMPD> mpd = media_manager_.get_mpd();
    assert(mpd != nullptr);

    configure_audio_representation_combo_box(get_audio_adaptation_set(get_period(mpd)));
    update_configuration(mpd);
}

void player::on_representation_combo_box_current_index_changed([[maybe_unused]] const int index)
{
    const std::shared_ptr<const dash::mpd::IMPD> mpd = media_manager_.get_mpd();
    assert(mpd != nullptr);

    update_configuration(mpd);
}

void player::on_download_mpd_button_clicked()
{
    const std::string url = get_url();
    if (url.starts_with("https://"))
    {
        SPDLOG_WARN("Streaming via HTTPS is currently not supported");
        return;
    }

    const std::shared_ptr<const dash::mpd::IMPD> old_mpd = media_manager_.get_mpd();

    if (media_manager_.initialize(url))
    {
        const std::shared_ptr<const dash::mpd::IMPD> mpd = media_manager_.get_mpd();
        update_gui(mpd);
        update_configuration(mpd);
    }
}

void player::on_start_button_clicked()
{
    if (media_manager_.start())
    {
        set_stop_button_enabled(true);
        set_start_button_enabled(false);
        set_download_mpd_button_enabled(false);
    }
}

void player::on_stop_button_clicked()
{
    media_manager_.stop();
    set_stop_button_enabled(false);
    set_start_button_enabled(true);
    set_download_mpd_button_enabled(true);
}

player::player()
    : QMainWindow(nullptr),
      ui_(make_gui()),
      ui_mutex_(*this),
      signals_mutex_(*this),
      gl_renderer_(ui_->video_render_widget),
      media_manager_(*gl_renderer_, audio_player_)
{
    connect(ui_->audio_representation_combo_box, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &player::on_representation_combo_box_current_index_changed);

    connect(ui_->video_representation_combo_box, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &player::on_representation_combo_box_current_index_changed);

    gl_renderer_->setEnabled(true);
    set_stop_button_enabled(false);
    update_gui();
}

player::ui_mutex::ui_mutex(player &instance)
    : instance_(instance)
{}

void player::ui_mutex::lock()
{
    instance_.setEnabled(false);
}

void player::ui_mutex::unlock()
{
    instance_.setEnabled(true);
}

player::signals_mutex::signals_mutex(player &instance)
    : instance_(instance)
{}

void player::signals_mutex::lock()
{
    instance_.set_signals_enabled(false);
}

void player::signals_mutex::unlock()
{
    instance_.set_signals_enabled(true);
}

std::string player::get_url()
{
    std::scoped_lock lock(ui_mutex_);
    const std::string mpd_url_text_edit = ui_->mpd_url_text_edit->toPlainText().toStdString();
    if (!mpd_url_text_edit.empty())
    {
        return mpd_url_text_edit;
    }

    return ui_->mpd_url_combo_box->currentText().toStdString();
}

const dash::mpd::IPeriod *player::get_period(std::shared_ptr<const dash::mpd::IMPD> mpd)
{
    if (mpd)
    {
        const std::vector<dash::mpd::IPeriod *> periods = mpd->GetPeriods();
        const int index = ui_->period_combo_box->currentIndex();
        if (index == -1)
        {
            return nullptr;
        }

        const auto period_index = static_cast<std::size_t>(index);
        if (period_index < periods.size())
        {
            return periods.at(period_index);
        }
    }

    return nullptr;
}

const dash::mpd::IAdaptationSet *player::get_video_adaptation_set(const dash::mpd::IPeriod *period)
{
    if (period)
    {
        const std::vector<const dash::mpd::IAdaptationSet *> video_adaptation_sets = get_video_adaptation_sets(period);
        const int index = ui_->video_as_combo_box->currentIndex();
        if (index == -1)
        {
            return nullptr;
        }

        const auto video_as_index = static_cast<std::size_t>(index);
        if (video_as_index < video_adaptation_sets.size())
        {
            return video_adaptation_sets.at(video_as_index);
        }
    }

    return nullptr;
}

const dash::mpd::IAdaptationSet *player::get_audio_adaptation_set(const dash::mpd::IPeriod *period)
{
    if (period)
    {
        const std::vector<const dash::mpd::IAdaptationSet *> audio_adaptation_sets = get_audio_adaptation_sets(period);
        const int index = ui_->audio_as_combo_box->currentIndex();
        if (index == -1)
        {
            return nullptr;
        }

        const auto video_as_index = static_cast<std::size_t>(index);
        if (video_as_index < audio_adaptation_sets.size())
        {
            return audio_adaptation_sets.at(video_as_index);
        }
    }

    return nullptr;
}

const dash::mpd::IRepresentation *player::get_video_representation(const dash::mpd::IAdaptationSet *adaptation_set)
{
    if (adaptation_set)
    {
        const std::vector<dash::mpd::IRepresentation *> &video_representations = adaptation_set->GetRepresentation();
        const int index = ui_->video_representation_combo_box->currentIndex();
        if (index == -1)
        {
            return nullptr;
        }

        const auto video_representation_index = static_cast<std::size_t>(index);
        if (video_representation_index < video_representations.size())
        {
            return video_representations.at(video_representation_index);
        }
    }

    return nullptr;
}

const dash::mpd::IRepresentation *player::get_audio_representation(const dash::mpd::IAdaptationSet *adaptation_set)
{
    if (adaptation_set)
    {
        const std::vector<dash::mpd::IRepresentation *> &audio_representations = adaptation_set->GetRepresentation();
        const int index = ui_->audio_representation_combo_box->currentIndex();
        if (index == -1)
        {
            return nullptr;
        }

        const auto audio_representation_index = static_cast<std::size_t>(index);
        if (audio_representation_index < audio_representations.size())
        {
            return audio_representations.at(audio_representation_index);
        }
    }

    return nullptr;
}

std::optional<media::manager::configuration> player::make_configuration(std::shared_ptr<const dash::mpd::IMPD> mpd)
{
    if (!mpd)
    {
        return {};
    }

    const dash::mpd::IPeriod *period = get_period(mpd);
    if (!period)
    {
        return {};
    }

    const dash::mpd::IAdaptationSet *const video_as = get_video_adaptation_set(period);
    const dash::mpd::IRepresentation *const video_representation = get_video_representation(video_as);
    const std::optional<media::manager::configuration::stream> video_stream =
        make_stream_configuration(video_as, video_representation);

    const dash::mpd::IAdaptationSet *const audio_as = get_audio_adaptation_set(period);
    const dash::mpd::IRepresentation *const audio_representation = get_audio_representation(audio_as);
    const std::optional<media::manager::configuration::stream> audio_stream =
        make_stream_configuration(audio_as, audio_representation);

    return media::manager::configuration
    {
        .period_ = *period,
        .video_stream_ = video_stream,
        .audio_stream_ = audio_stream
    };
}

void player::update_configuration(std::shared_ptr<const dash::mpd::IMPD> mpd)
{
    if (const std::optional<media::manager::configuration> configuration = make_configuration(mpd))
    {
        media_manager_.set_configuration(configuration.value());

        if (const std::optional<media::manager::configuration::stream> video_stream = configuration->video_stream_)
        {
            const dash::mpd::IRepresentation &video_representation = video_stream->representation_;

            const std::string frame_rate = video_representation.GetFrameRate();
            gl_renderer_->set_frame_rate(frame_rate.empty() ? 24 : std::stoi(frame_rate));
        }
    }
}

std::shared_ptr<Ui::qt_player> player::make_gui()
{
    const auto ui = std::make_shared<Ui::qt_player>();
    ui->setupUi(this);
    return ui;
}

void player::set_start_button_enabled(const bool enabled)
{
    std::scoped_lock lock(ui_mutex_);
    ui_->start_button->setEnabled(enabled);
}

void player::set_stop_button_enabled(const bool enabled)
{
    std::scoped_lock lock(ui_mutex_);
    ui_->stop_button->setEnabled(enabled);
}

void player::set_download_mpd_button_enabled(const bool enabled)
{
    std::scoped_lock lock(ui_mutex_);
    ui_->download_mpd_button->setEnabled(enabled);
}

void player::set_signals_enabled(const bool enabled)
{
    std::scoped_lock lock(ui_mutex_);
    for (QWidget *const widget : findChildren<QWidget*>())
    {
        widget->blockSignals(!enabled);
    }
}

void player::on_period_changed(const dash::mpd::IPeriod *period)
{
    const std::vector<const dash::mpd::IAdaptationSet *> video_adaptation_sets = get_video_adaptation_sets(period);
    const std::vector<const dash::mpd::IAdaptationSet *> audio_adaptation_sets = get_audio_adaptation_sets(period);

    configure_adaptation_set_combo_boxes(video_adaptation_sets, audio_adaptation_sets);

    const dash::mpd::IAdaptationSet *default_video_adaptation_set =
        video_adaptation_sets.empty() ? nullptr : video_adaptation_sets.front();
    configure_video_representation_combo_box(default_video_adaptation_set);

    const dash::mpd::IAdaptationSet *default_audio_adaptation_set =
        audio_adaptation_sets.empty() ? nullptr : audio_adaptation_sets.front();
    configure_audio_representation_combo_box(default_audio_adaptation_set);
}

void player::update_gui(std::shared_ptr<const dash::mpd::IMPD> mpd)
{
    set_start_button_enabled(mpd != nullptr);

    const auto &periods = mpd ? mpd->GetPeriods() : std::vector<dash::mpd::IPeriod *>{};
    configure_period_combo_box(periods);

    const dash::mpd::IPeriod *default_period = periods.empty() ? nullptr : periods.front();
    on_period_changed(default_period);
}

void player::configure_period_combo_box(const std::vector<dash::mpd::IPeriod *> &periods)
{
    std::scoped_lock lock(ui_mutex_);
    std::scoped_lock signals_lock(signals_mutex_);

    QComboBox *const period_combo_box = ui_->period_combo_box;
    period_combo_box->clear();
    period_combo_box->setEnabled(periods.size() > 1);

    for (std::size_t index = 0; index < periods.size(); ++index)
    {
        const QString label(fmt::format("Period {}", index).c_str());
        period_combo_box->addItem(label);
    }
}

void player::configure_adaptation_set_combo_boxes(
        const std::vector<const dash::mpd::IAdaptationSet *> &video_adaptation_sets,
        const std::vector<const dash::mpd::IAdaptationSet *> &audio_adaptation_sets)
{
    std::scoped_lock ui_lock(ui_mutex_);
    std::scoped_lock signals_lock(signals_mutex_);

    QComboBox *const video_as_combo_box = ui_->video_as_combo_box;
    QComboBox *const audio_as_combo_box = ui_->audio_as_combo_box;

    video_as_combo_box->clear();
    video_as_combo_box->setEnabled(video_adaptation_sets.size() > 1);
    audio_as_combo_box->clear();
    audio_as_combo_box->setEnabled(audio_adaptation_sets.size() > 1);

    for (std::size_t index = 0; index < video_adaptation_sets.size(); ++index)
    {
        const QString label(fmt::format("Adaptation set {}", index).c_str());
        video_as_combo_box->addItem(label);
    }

    for (std::size_t index = 0; index < audio_adaptation_sets.size(); ++index)
    {
        const QString label(fmt::format("Adaptation set {}", index).c_str());
        audio_as_combo_box->addItem(label);
    }
}

void player::configure_video_representation_combo_box(const dash::mpd::IAdaptationSet *video_adaptation_set)
{
    std::scoped_lock ui_lock(ui_mutex_);
    std::scoped_lock signals_lock(signals_mutex_);

    QComboBox *const video_representation_combo_box = ui_->video_representation_combo_box;

    const auto &video_representations =
        video_adaptation_set ? video_adaptation_set->GetRepresentation() : std::vector<dash::mpd::IRepresentation*>{};

    video_representation_combo_box->clear();
    video_representation_combo_box->setEnabled(video_representations.size() > 1);

    for (const dash::mpd::IRepresentation *representation : video_representations)
    {
        const QString label(fmt::format("{}fps {}bps {}x{}",
            representation->GetFrameRate(), representation->GetBandwidth(),
            representation->GetWidth(), representation->GetHeight()).c_str());
        video_representation_combo_box->addItem(label);
    }
}

void player::configure_audio_representation_combo_box(const dash::mpd::IAdaptationSet *audio_adaptation_set)
{
    std::scoped_lock ui_lock(ui_mutex_);
    std::scoped_lock signals_lock(signals_mutex_);

    QComboBox *const audio_representation_combo_box = ui_->audio_representation_combo_box;

    const auto &audio_representations =
        audio_adaptation_set ? audio_adaptation_set->GetRepresentation() : std::vector<dash::mpd::IRepresentation*>{};

    audio_representation_combo_box->clear();
    audio_representation_combo_box->setEnabled(audio_representations.size() > 1);

    for (const dash::mpd::IRepresentation *representation : audio_representations)
    {
        const QString label(fmt::format("{}bps", representation->GetBandwidth()).c_str());
        audio_representation_combo_box->addItem(label);
    }
}

} // namespace ms::qt
