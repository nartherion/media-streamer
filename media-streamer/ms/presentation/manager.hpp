#pragma once

#include <ms/presentation/frame_renderer.hpp>
#include <ms/utils/frame_acceptor.hpp>
#include <ms/common/buffer.hpp>

#include <thread>

namespace ms::presentation
{

class manager : public utils::frame_acceptor
{
public:
    manager(frame_renderer &renderer, std::size_t frame_buffer_size);
    ~manager();

    bool start();
    void stop();

private:
    using frame_buffer = common::buffer<std::optional<av::frame>>;

    void do_rendering();
    void accept(av::frame frame) override;

    const std::size_t frame_buffer_size_;
    frame_renderer &frame_renderer_;
    std::optional<frame_buffer> frames_;
    int frame_rate_ = 0;
    std::thread rendering_thread_;
    std::atomic<bool> is_rendering_ = false;
};

} // namespace ms::presentation
