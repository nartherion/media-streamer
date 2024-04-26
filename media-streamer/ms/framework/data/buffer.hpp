#pragma once

#include <ms/common/immobile.hpp>
#include <ms/framework/data/object.hpp>

#include <cstdint>

#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace ms::framework::data
{

class buffer : public immobile
{
public:
    buffer(std::size_t capacity);
    ~buffer();

    bool push(std::shared_ptr<object> object);
    std::shared_ptr<object> pop();
    void set_eos();

private:
    const std::size_t capacity_;
    std::queue<std::shared_ptr<object>> objects_;
    bool is_eos_ = false;
    mutable std::mutex buffer_mutex_;
    mutable std::condition_variable buffer_cv_;
};

} // namespace ms::framework::data
