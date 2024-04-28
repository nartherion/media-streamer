#pragma once

#include <ms/common/immobile.hpp>
#include <ms/framework/mpd/adaptation_set_stream.hpp>
#include <ms/framework/mpd/representation_stream.hpp>
#include <ms/common/buffer.hpp>
#include <ms/framework/data/decoder_events_handler.hpp>

#include <cstdint>

#include <map>
#include <mutex>
#include <atomic>
#include <optional>
#include <memory>

#include <gsl/pointers>

#include <IMPD.h>
#include <IPeriod.h>
#include <IRepresentation.h>
#include <IAdaptationSet.h>

namespace ms::framework::stream
{

class receiver : public common::immobile, public data::decoder_events_handler
{
public:
    receiver(const dash::mpd::IMPD &mpd, std::size_t buffer_size);
    ~receiver();

    bool start();
    void stop();
    void set_representation(const dash::mpd::IPeriod &period, const dash::mpd::IAdaptationSet &adaptation_set,
                            const dash::mpd::IRepresentation &representation);
    std::shared_ptr<data::object> get_oldest_segment() override;
    std::shared_ptr<data::object> find_initialization_segment(
        const dash::mpd::IRepresentation &representation) const override;

private:
    using period_pointer = gsl::not_null<const dash::mpd::IPeriod *>;
    using adaptation_set_pointer = gsl::not_null<const dash::mpd::IAdaptationSet *>;
    using representation_pointer = gsl::not_null<const dash::mpd::IRepresentation *>;
    using initialization_segments_table = std::map<representation_pointer, std::shared_ptr<data::object>>;
    using object_buffer = common::buffer<std::shared_ptr<data::object>>;

    std::shared_ptr<data::object> get_next_segment();
    std::shared_ptr<data::object> get_initialization_segment();
    std::uint32_t calculate_segment_offset() const;
    void download_initialization_segment();
    void do_buffering();

    const dash::mpd::IMPD &mpd_;
    const std::size_t buffer_size_;
    std::optional<object_buffer> buffer_;
    initialization_segments_table initialization_segments_;
    period_pointer period_;
    adaptation_set_pointer adaptation_set_;
    representation_pointer representation_;
    std::optional<mpd::adaptation_set_stream> adaptation_set_stream_;
    std::shared_ptr<mpd::representation_stream> representation_stream_;
    std::uint32_t segment_number_ = 0;
    std::uint32_t segment_offset_ = 0;
    std::thread buffering_thread_;
    mutable std::mutex monitor_mutex_;
    std::atomic<bool> settings_updated_ = true;
    std::atomic<bool> is_buffering_ = false;
};

} // namespace ms::framework::stream
