#pragma once

#include <ms/common/immobile.hpp>

#include <cstdint>
#include <cassert>

#include <queue>
#include <optional>
#include <mutex>
#include <condition_variable>

namespace ms::common
{

template <typename T>
class buffer : public immobile
{
public:
    buffer(std::size_t capacity);
    ~buffer();

    bool push(T object);
    T pop();
    void clear();
    void set_eos();

private:
    const std::size_t capacity_;
    std::queue<T> objects_;
    bool is_eos_ = false;
    std::mutex buffer_mutex_;
    std::condition_variable buffer_cv_;
};

template <typename T>
buffer<T>::buffer(const std::size_t capacity)
    : capacity_(capacity)
{}

template <typename T>
buffer<T>::~buffer()
{
    set_eos();
}

template <typename T>
bool buffer<T>::push(T object)
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
    objects_.push(std::move(object));
    buffer_cv_.notify_all();
    return true;
}

template <typename T>
T buffer<T>::pop()
{
    std::unique_lock lock(buffer_mutex_);

    if (objects_.empty())
    {
        buffer_cv_.wait(lock, [this] { return !objects_.empty() || is_eos_; });
    }

    if (is_eos_)
    {
        return {};
    }

    assert(!objects_.empty() && !is_eos_);
    T object = std::move(objects_.front());
    objects_.pop();
    buffer_cv_.notify_all();
    return object;
}

template <typename T>
void buffer<T>::set_eos()
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

} // namespace ms::common
