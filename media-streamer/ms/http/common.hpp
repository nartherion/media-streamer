#pragma once

#include <string>

namespace ms::http
{

struct client_config
{
    std::string host;
    std::string port;
    std::string target;
    unsigned version;
};

} // namespace ms::http
