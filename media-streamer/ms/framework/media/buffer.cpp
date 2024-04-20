#include <ms/framework/media/buffer.hpp>

#include <cassert>

namespace ms::framework::media
{

buffer::buffer(const std::uint32_t capacity)
    : capacity_(capacity)
{}

buffer::~buffer()
{
    clear();
}

bool buffer::push(const std::shared_ptr<object> o)
{
    std::unique_lock lock(buffer_mutex_);

    if (objects_.size() == capacity_)
    {
        buffer_cv_.wait(lock, [this] { return objects_.empty() || is_eos_; });
    }

    if (is_eos_)
    {
        return false;
    }

    assert(objects_.size() < capacity_ && !is_eos_);
    objects_.push(o);
    buffer_cv_.notify_all();
    return true;
}

std::shared_ptr<object> buffer::pop()
{
    std::unique_lock lock(buffer_mutex_);

    if (objects_.empty())
    {
        buffer_cv_.wait(lock, [this] { return !objects_.empty() || is_eos_; });
    }

    if (is_eos_)
    {
        return nullptr;
    }

    assert(!objects_.empty() && !is_eos_);
    const std::shared_ptr<object> o = objects_.front();
    objects_.pop();
    buffer_cv_.notify_all();
    return o;
}

std::uint32_t buffer::size() const
{
    std::scoped_lock lock(buffer_mutex_);
    return static_cast<std::uint32_t>(objects_.size());
}

void buffer::set_eos()
{
    std::scoped_lock lock(buffer_mutex_);
    while (!objects_.empty())
    {
        objects_.pop();
    }

    is_eos_ = true;
    buffer_cv_.notify_all();
}

void buffer::clear()
{
    std::scoped_lock lock(buffer_mutex_);
    while (!objects_.empty())
    {
        objects_.pop();
    }

    buffer_cv_.notify_all();
}

std::uint32_t buffer::capacity() const
{
    return capacity_;
}

} // namespace ms::framework::media
