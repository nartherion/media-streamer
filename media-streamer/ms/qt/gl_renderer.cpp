#include <ms/qt/gl_renderer.hpp>

extern "C"
{

#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

} // extern "C"

#include <thread>

#include <spdlog/spdlog.h>

namespace ms::qt
{

namespace
{

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

gl_renderer::gl_renderer(QWidget *parent)
    : QGLWidget(parent)
{
    setAutoBufferSwap(true);
    setAutoFillBackground(true);
}

void gl_renderer::render(av::frame frame)
{
    set_frame(to_rgb(std::move(frame)));
    update();

    if (frame_rate_ != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / frame_rate_));
    }
}

void gl_renderer::set_frame_rate(const int frame_rate)
{
    frame_rate_ = frame_rate;
}

void gl_renderer::paintEvent(QPaintEvent *)
{
    std::scoped_lock lock(image_mutex_);

    QPainter painter;
    painter.begin(this);
    painter.drawImage(rect(), image_);
    painter.end();
}

void gl_renderer::set_frame(av::frame frame)
{
    std::scoped_lock lock(image_mutex_);

    AVFrame *av_frame = frame.native();
    const int width = av_frame->width;
    const int height = av_frame->height;

    std::uint8_t *source = av_frame->data[0];

    QImage image(width, height, QImage::Format_RGB32);
    for (int y = 0; y < height; ++y)
    {
        auto *const scan_line = reinterpret_cast<QRgb *>(image.scanLine(y));

        for (int x = 0; x < width; ++x)
        {
            const auto offset = static_cast<std::size_t>(3 * x);
            scan_line[x] = qRgb(source[offset], source[offset + 1], source[offset + 2]);
        }

        source += av_frame->linesize[0];
    }

    image_ = image;
}

} // ms::qt
