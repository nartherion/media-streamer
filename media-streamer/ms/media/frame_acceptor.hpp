#pragma once

#include <cstdint>

namespace ms::media
{

struct video_frame
{
    int width_ = 0;
    int height_ = 0;
    int format_ = 0;
    int *linesize_ = nullptr;
    const std::uint8_t **data_ = nullptr;
};

struct audio_frame
{
    int linesize_ = 0;
    int sample_rate_ = 0;
    int channels_ = 0;
    const std::uint8_t **data_ = nullptr;
};

class frame_acceptor
{
public:
    virtual void accept(video_frame frame) = 0;
    virtual void accept(audio_frame frame) = 0;

    virtual ~frame_acceptor() = default;
};

} // namespace ms::media
