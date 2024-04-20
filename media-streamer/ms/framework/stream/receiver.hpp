#pragma once

#include <ms/framework/mpd/adaptation_set_stream.hpp>
#include <ms/framework/mpd/representation_stream.hpp>
#include <ms/framework/media/buffer.hpp>

#include <cstdint>

#include <map>
#include <mutex>
#include <atomic>

#include <gsl/pointers>

#include <IMPD.h>
#include <IPeriod.h>
#include <IRepresentation.h>
#include <IAdaptationSet.h>

namespace ms::framework::stream
{

class receiver
{
public:
    receiver(const dash::mpd::IMPD &mpd, std::uint32_t buffer_capacity);
    ~receiver();

    bool start();
    void stop();
    std::shared_ptr<media::object> get_next_segment();
    std::shared_ptr<media::object> get_segment(std::uint32_t segment_number);
    std::shared_ptr<media::object> get_initialization_segment();
    std::shared_ptr<media::object> find_initialization_segment(const dash::mpd::IRepresentation &representation);
    std::uint32_t get_position() const;
    void set_position(std::uint32_t segment_number);
    void set_position_in_milliseconds(std::uint32_t milliseconds);
    const dash::mpd::IRepresentation &get_representation() const;
    void set_representation(const dash::mpd::IPeriod &period, const dash::mpd::IAdaptationSet &adaptation_set,
                            const dash::mpd::IRepresentation &representation);

private:
    using initialiation_segments_table =
        std::map<gsl::not_null<const dash::mpd::IRepresentation *>, std::shared_ptr<media::object>>;

    std::uint32_t calculate_segment_offset() const;
    void download_initialization_segment(const dash::mpd::IRepresentation &representation);
    void initialization_segment_exists(const dash::mpd::IRepresentation &representation);
    void do_buffering();

    const dash::mpd::IMPD &mpd_;
    gsl::not_null<const dash::mpd::IPeriod *> period_;
    gsl::not_null<const dash::mpd::IAdaptationSet *> adaptation_set_;
    gsl::not_null<const dash::mpd::IRepresentation *> representation_;
    initialiation_segments_table initialization_segments_;
    std::shared_ptr<mpd::adaptation_set_stream> adaptation_set_stream_;
    std::shared_ptr<mpd::representation_stream> representation_stream_;
    std::uint32_t segment_number_ = 0;
    std::uint32_t position_in_milliseconds_ = 0;
    std::uint32_t segment_offset_ = 0;
    media::buffer buffer_;
    std::thread buffering_thread_;
    std::atomic<bool> is_buffering_ = false;
};

} // namespace ms::framework::stream
