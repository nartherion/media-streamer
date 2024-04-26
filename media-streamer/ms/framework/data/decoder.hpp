#pragma once

#include <ms/framework/data/object.hpp>
#include <ms/framework/data/decoder_events_handler.hpp>
#include <ms/decoder/decoder.hpp>

#include <thread>

namespace ms::framework::data
{

class decoder final : public packet_provider
{
public:
    static std::optional<decoder> create_and_start(std::shared_ptr<object> initialization_segment,
                                                   std::shared_ptr<object> media_segment,
                                                   decoder_events_handler &events_handler);

private:
    decoder(std::shared_ptr<object> initialization_segment, std::shared_ptr<object> media_segment,
            decoder_events_handler &events_handler);

    static void decode(media::decoder media_decoder);

    int read_packet(std::span<std::byte> buffer) override;
    bool initialize_decoding_thread();

    const std::shared_ptr<object> initialization_segment_;
    const std::shared_ptr<object> media_segment_;
    decoder_events_handler &events_handler_;
    std::optional<media::decoder> decoder_;
    std::jthread decoding_thread_;
    bool initialization_segment_peeked_ = false;
};

} // namespace ms::framework::data
