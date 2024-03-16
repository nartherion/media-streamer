#pragma once

#include <ms/http/common.hpp>

#include <memory>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace ms::http
{

class client : public std::enable_shared_from_this<client>
{
public:
    using http_request_t = boost::beast::http::request<boost::beast::http::empty_body>;
    static void create_and_start(boost::asio::io_context &context, const client_config &config);

private:
    explicit client(boost::asio::io_context &context);

    void run(const client_config &config);
    void on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);
    void on_connect(boost::beast::error_code ec);
    void on_write(boost::beast::error_code ec);
    void on_read(boost::beast::error_code ec);

    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::tcp_stream stream_;
    boost::beast::flat_buffer buffer_;
    http_request_t request_;
    boost::beast::http::response<boost::beast::http::string_body> response_;
};

} // namespace ms::http
