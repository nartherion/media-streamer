#pragma once

#include <ms/framework/data/object.hpp>
#include <ms/framework/data/buffer.hpp>
#include <ms/framework/data/decoder_events_handler.hpp>
#include <ms/framework/stream/receiver.hpp>
#include <ms/decoder/decoder.hpp>

#include <thread>

namespace ms::framework::data
{

class decoder final : public packet_provider
{
public:
    decoder(data::buffer &buffer, decoder_events_handler &events_handler);
    ~decoder();

    bool start();
    void stop();

private:
    int read_packet(std::span<std::byte> buffer) override;
    void do_decoding();

    data::buffer &buffer_;
    decoder_events_handler &events_handler_;
    std::shared_ptr<object> media_segment_;
    std::shared_ptr<object> initialization_segment_;
    std::thread decoding_thread_;
    std::atomic<bool> is_decoding_ = false;
};

} // namespace ms::framework::data
