#include <ms/presentation/manager.hpp>

namespace ms::presentation
{

manager::manager(frame_renderer &renderer, const std::size_t frame_buffer_size)
    : frame_buffer_size_(frame_buffer_size),
      frame_renderer_(renderer)
{}

manager::~manager()
{
    stop();
}

bool manager::start()
{
    if (is_rendering_.load())
    {
        return false;
    }

    frames_.emplace(frame_buffer_size_);
    is_rendering_.store(true);
    rendering_thread_ = std::thread([this] { do_rendering(); });
    return true;
}

void manager::stop()
{
    if (!is_rendering_.load())
    {
        return;
    }

    frames_->set_eos();
    is_rendering_.store(false);
    if (rendering_thread_.joinable())
    {
        rendering_thread_.join();
    }
}

void manager::do_rendering()
{
    while (is_rendering_.load())
    {
        while (std::optional<av::frame> frame = frames_->pop())
        {
            frame_renderer_.render(std::move(frame.value()));
        }
    }

    frames_->set_eos();
}

void manager::accept(av::frame frame)
{
    frames_->push(std::move(frame));
}

} // namespace ms::presentation
