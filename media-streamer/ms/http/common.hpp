#pragma once

#include <string>

#include <boost/beast/http/verb.hpp>

namespace ms::http
{

struct client_config
{
    struct endpoint
    {
        std::string host_;
        int port_;
    };
    endpoint endpoint_;

    struct request
    {
        std::string target_;
        boost::beast::http::verb method_;
    };
    request request_;
};

} // namespace ms::http
