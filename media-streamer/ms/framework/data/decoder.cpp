#include <ms/framework/data/decoder.hpp>

#include <spdlog/spdlog.h>

namespace ms::framework::data
{

decoder::decoder(decoder_events_handler &events_handler, utils::frame_acceptor &frame_acceptor)
    : frame_acceptor_(frame_acceptor),
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
        media_segment_ = events_handler_.get_oldest_segment();
        if (!media_segment_)
        {
            continue;
        }

        initialization_segment_ = events_handler_.find_initialization_segment(media_segment_->get_representation());

        std::optional<utils::decoder> media_decoder = utils::decoder::create(*this);
        if (!media_decoder.has_value())
        {
            SPDLOG_ERROR("Failed to create media decoder");
            continue;
        }

        while (is_decoding_.load())
        {
            std::optional<av::frame> frame = media_decoder->get_frame();
            if (!frame.has_value())
            {
                break;
            }

            frame_acceptor_.accept(std::move(frame.value()));
        }
    }
}

} // namespace ms::framework::data
