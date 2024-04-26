#pragma once

#include <cstddef>

#include <span>

namespace ms::framework::data
{

class packet_provider
{
public:
    virtual int read_packet(std::span<std::byte> buffer) = 0;

    virtual ~packet_provider() = default;
};

} // namespace ms::framework::data
