#include <ms/framework/data/decoder.hpp>

#include <spdlog/spdlog.h>

namespace ms::framework::data
{

std::optional<decoder> decoder::create_and_start(const std::shared_ptr<object> initialization_segment,
                                                 const std::shared_ptr<object> media_segment,
                                                 decoder_events_handler &events_handler)
{
    decoder instance(initialization_segment, media_segment, events_handler);
    if (instance.initialize_decoding_thread())
    {
        SPDLOG_ERROR("Failed to initialize decoding thread");
        return instance;
    }

    return {};
}

decoder::decoder(const std::shared_ptr<object> initialization_segment, const std::shared_ptr<object> media_segment,
                 decoder_events_handler &events_handler)
    : initialization_segment_(initialization_segment),
      media_segment_(media_segment),
      events_handler_(events_handler)
{}

void decoder::decode(media::decoder media_decoder)
{
    media_decoder.decode();
}

int decoder::read_packet(std::span<std::byte> buffer)
{
    if (!initialization_segment_peeked_)
    {
        initialization_segment_peeked_ = true;
        return initialization_segment_->peek(buffer);
    }

    return media_segment_->read(buffer);
}

bool decoder::initialize_decoding_thread()
{
    std::optional<media::decoder> media_decoder = media::decoder::create(events_handler_, *this);
    if (!media_decoder.has_value())
    {
        SPDLOG_ERROR("Failed to create media decoder");
        return false;
    }

    decoding_thread_ = std::jthread(decode, std::move(media_decoder.value()));
    return true;
}

} // namespace ms::framework::data
