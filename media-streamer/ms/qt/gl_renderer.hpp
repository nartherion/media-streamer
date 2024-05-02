#pragma once

#include <ms/presentation/frame_renderer.hpp>

#include <mutex>

#include <QtOpenGL/QGLWidget>

namespace ms::qt
{

class gl_renderer : public QGLWidget, public presentation::frame_renderer
{
    Q_OBJECT

public:
    gl_renderer(QWidget *parent);

    void render(av::frame frame) override;
    void set_frame_rate(int frame_rate);

protected:
    void paintEvent(QPaintEvent *) override;
    void set_frame(av::frame frame);

private:
    QImage image_;
    int frame_rate_ = 0;
    std::mutex image_mutex_;
};

} // namespace ms::qt
