#include <ms/framework/data/decoder.hpp>

#include <spdlog/spdlog.h>

namespace ms::framework::data
{

decoder::decoder(data::buffer &buffer, decoder_events_handler &events_handler)
    : buffer_(buffer),
      events_handler_(events_handler)
{}

decoder::~decoder()
{
    stop();
}

bool decoder::start()
{
    if (is_decoding_.load())
    {
        return false;
    }

    is_decoding_.store(true);
    decoding_thread_ = std::thread([this] { do_decoding(); });
    return true;
}

void decoder::stop()
{
    if (!is_decoding_.load())
    {
        return;
    }

    is_decoding_.store(false);
    if (decoding_thread_.joinable())
    {
        decoding_thread_.join();
    }
}

int decoder::read_packet(std::span<std::byte> buffer)
{
    if (initialization_segment_)
    {
        return std::exchange(initialization_segment_, {})->peek(buffer);
    }

    return media_segment_->read(buffer);
}

void decoder::do_decoding()
{
    while (is_decoding_.load())
    {
        media_segment_ = events_handler_.get_next_segment();
        if (!media_segment_)
        {
            continue;
        }

        initialization_segment_ = events_handler_.find_initialization_segment(media_segment_->get_representation());
        if (!initialization_segment_)
        {
            continue;
        }

        std::optional<media::decoder> media_decoder = media::decoder::create(events_handler_, *this);
        if (!media_decoder.has_value())
        {
            SPDLOG_ERROR("Failed to create media decoder");
            continue;
        }

        media_decoder->decode();
    }
}

} // namespace ms::framework::data
