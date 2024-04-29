#include <ms/media/decoder.hpp>
#include <ms/av/error.hpp>

extern "C"
{

#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

} // extern "C"

#include <spdlog/spdlog.h>

namespace ms::media
{

namespace
{

static int read_packet(void *const opaque, std::uint8_t *const buffer, const int buffer_size)
{
    auto *const provider = reinterpret_cast<framework::data::packet_provider *>(opaque);
    const auto buffer_view = std::span(reinterpret_cast<std::byte *>(buffer), static_cast<std::size_t>(buffer_size));
    return provider->read_packet(buffer_view);
}

av::frame to_rgb(av::frame frame)
{
    const AVFrame *source_frame = frame.native();
    const auto source_format = static_cast<AVPixelFormat>(source_frame->format);

    const AVPixelFormat destination_format = AV_PIX_FMT_RGB24;
    const int width = source_frame->width;
    const int height = source_frame->height;

    av::frame rgb_frame;
    AVFrame *destination_frame = rgb_frame.native();
    destination_frame->width = width;
    destination_frame->height = height;
    destination_frame->format = destination_format;

    av_image_alloc(destination_frame->data, destination_frame->linesize, width, height, destination_format, 1);

    SwsContext *const conversion = sws_getContext(width, height, source_format,
                                                  width, height, destination_format,
                                                  SWS_BICUBIC, NULL, NULL, NULL);
    sws_scale(conversion, source_frame->data, source_frame->linesize, 0, height,
              destination_frame->data, destination_frame->linesize);
    sws_freeContext(conversion);

    return rgb_frame;
}

} // namespace

std::optional<decoder> decoder::create(frame_acceptor &acceptor, framework::data::packet_provider &provider)
{
    constexpr int buffer_size = 1 << 15;
    auto *const opaque = reinterpret_cast<void *>(&provider);
    std::optional<av::format_context> format_context = av::format_context::create(buffer_size, opaque, read_packet);
    if (!format_context.has_value())
    {
        SPDLOG_ERROR("Failed to create decoder: Can't create format context");
        return {};
    }

    return decoder(std::move(format_context.value()), acceptor);
}

decoder::decoder(av::format_context format_context, frame_acceptor &acceptor)
    : format_context_(std::move(format_context)),
      codecs_(make_codecs()),
      acceptor_(acceptor)
{}

std::vector<av::codec_context> decoder::make_codecs()
{
    const AVFormatContext *av_format_context = format_context_.native();
    std::vector<av::codec_context> codecs;
    for (std::size_t index = 0; index < av_format_context->nb_streams; ++index)
    {
        const AVStream *av_stream = av_format_context->streams[index];
        const AVCodecParameters* codecpar = av_stream->codecpar;
        const AVMediaType codec_type = codecpar->codec_type;
        if ((codec_type == AVMEDIA_TYPE_VIDEO) || (codec_type == AVMEDIA_TYPE_AUDIO))
        {
            if (AVCodec *av_codec = avcodec_find_decoder(codecpar->codec_id))
            {
                const std::optional<av::codec_context> codec = av::codec_context::create(av_codec, codecpar, av_stream);
                if (codec.has_value())
                {
                    codecs.push_back(codec.value());
                }
            }
        }
    }
    return codecs;
}

void decoder::notify(av::frame frame)
{
    const std::optional<av::codec_context> codec = frame.get_codec();
    if (!codec.has_value())
    {
        SPDLOG_ERROR("Failed to notify frame acceptor: Codec is missing");
        return;
    }

    const AVMediaType codec_type = codec->native()->codec_type;
    switch (codec_type)
    {
    case AVMEDIA_TYPE_VIDEO:
        notify_video(to_rgb(frame));
        break;

    case AVMEDIA_TYPE_AUDIO:
        notify_audio(frame);
        break;

    default:
        SPDLOG_ERROR("Failed to notify frame acceptor: Codec type {} is not supported",
                     av_get_media_type_string(codec_type));
        break;
    }
}

void decoder::notify_video(av::frame frame)
{
    AVFrame *av_frame = frame.native();
    const auto format = static_cast<AVPixelFormat>(av_frame->format);
    if (format != AV_PIX_FMT_RGB24)
    {
        SPDLOG_ERROR("Failed to notify frame acceptor: Format {} is not supported", av_get_pix_fmt_name(format));
        return;
    }

    acceptor_.accept_video(frame);
}

void decoder::notify_audio(av::frame frame)
{
    acceptor_.accept_audio(frame);
}

std::optional<av::packet> decoder::get_next_packet()
{
    av::packet packet;

    int error = 0;
    while ((error = av_read_frame(format_context_.native(), packet.native())) >= 0)
    {
        if (const std::optional<av::codec_context> codec = find_codec(packet.native()->stream_index))
        {
            packet.set_codec(codec.value());
            return packet;
        }

        packet = {};
    }

    if (error < 0 && error != AVERROR_EOF)
    {
        SPDLOG_ERROR("Failed to read the packet: {}", av::make_error_string(error));
    }

    return {};
}

std::optional<av::codec_context> decoder::find_codec(const int index)
{
    const auto entry = std::find_if(codecs_.begin(), codecs_.end(),
        [index](const av::codec_context codec) { return codec.stream()->index == index; });
    if (entry != codecs_.end())
    {
        return *entry;
    }

    return {};
}

bool decoder::decode_media(const av::packet packet, av::frame &frame)
{
    std::optional<av::codec_context> codec = frame.get_codec();
    if (!codec.has_value())
    {
        SPDLOG_ERROR("Failed to decode media: Codec is missing");
        return false;
    }

    if (const int error = avcodec_send_packet(codec->native(), packet.native()); error < 0)
    {
        SPDLOG_ERROR("Failed to send packet: {}", av::make_error_string(error));
        return false;
    }

    if (const int error = avcodec_receive_frame(codec->native(), frame.native()); error != 0)
    {
        if (error != AVERROR(EAGAIN))
        {
            SPDLOG_ERROR("Failed to receive frame: {}", av::make_error_string(error));
        }
        return false;
    }

    return true;
}

std::optional<av::frame> decoder::decode_frame(const av::packet packet)
{
    std::optional<av::codec_context> codec = packet.get_codec();
    if (!codec.has_value())
    {
        SPDLOG_ERROR("Failed to decode frame: Codec not available");
        return {};
    }

    av::frame frame;
    frame.set_codec(codec.value());
    if (decode_media(packet, frame))
    {
        return frame;
    }

    return {};
}

void decoder::decode()
{
    while (const std::optional<av::packet> packet = get_next_packet())
    {
        if (std::optional<av::frame> frame = decode_frame(packet.value()))
        {
            notify(frame.value());
        }
    }
}

} // namespace ms::media
