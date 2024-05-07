#pragma once

#include <ms/framework/data/object.hpp>
#include <ms/framework/data/decoder_events_handler.hpp>
#include <ms/framework/stream/receiver.hpp>
#include <ms/utils/decoder.hpp>

#include <thread>

namespace ms::framework::data
{

class decoder final : public packet_provider
{
public:
    decoder(decoder_events_handler &events_handler, utils::frame_acceptor &frame_acceptor);
    ~decoder();

    bool start();
    void stop();

private:
    int read_packet(std::span<std::byte> buffer) override;
    void do_decoding();

    utils::frame_acceptor &frame_acceptor_;
    decoder_events_handler &events_handler_;
    std::shared_ptr<object> media_segment_;
    std::shared_ptr<object> initialization_segment_;
    std::optional<utils::decoder> media_decoder_;
    std::thread decoding_thread_;
    std::atomic<bool> is_decoding_ = false;
};

} // namespace ms::framework::data
