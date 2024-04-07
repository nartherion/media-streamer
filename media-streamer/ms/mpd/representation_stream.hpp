#pragma once

#include <cstdint>

#include <ISegment.h>

namespace ms::mpd
{

class representation_stream
{
public:
    enum class type
    {
        undefined,
        single_media_segment,
        segment_list,
        segment_template
    };

    virtual ~representation_stream() = default;

    [[nodiscard]] virtual dash::mpd::ISegment *get_initialization_segment() const = 0;
    [[nodiscard]] virtual dash::mpd::ISegment *get_index_segment() const = 0;
    [[nodiscard]] virtual dash::mpd::ISegment *get_media_segment() const = 0;
    [[nodiscard]] virtual dash::mpd::ISegment *get_bitstream_switching_segment() const = 0;
    [[nodiscard]] virtual type get_stream_type() const = 0;
    [[nodiscard]] virtual std::uint32_t get_size() const = 0;
    [[nodiscard]] virtual std::uint32_t get_first_segment_number() const = 0;
    [[nodiscard]] virtual std::uint32_t get_current_segment_number() const = 0;
    [[nodiscard]] virtual std::uint32_t get_last_segment_number() const = 0;
    [[nodiscard]] virtual std::uint32_t get_average_segment_duration() const = 0;
};

} // namespace ms::mpd
