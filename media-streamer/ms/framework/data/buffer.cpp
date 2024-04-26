#include <ms/framework/data/buffer.hpp>

#include <cassert>

namespace ms::framework::data
{

buffer::buffer(const std::size_t capacity)
    : capacity_(capacity)
{}

buffer::~buffer()
{
    set_eos();
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

void buffer::set_eos()
{
    std::scoped_lock lock(buffer_mutex_);
    if (is_eos_)
    {
        return;
    }

    while (!objects_.empty())
    {
        objects_.pop();
    }

    is_eos_ = true;
    buffer_cv_.notify_all();
}

} // namespace ms::framework::data
