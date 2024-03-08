#pragma once

#include <memory>
#include <functional>

#include <boost/asio/ip/tcp.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace ms::http
{

class session : public std::enable_shared_from_this<session>
{
public:
    using http_request_t = boost::beast::http::request<boost::beast::http::string_body>;
    using on_http_request_callback_t = std::function<boost::beast::http::message_generator(http_request_t)>;

    static void create_and_start(boost::asio::ip::tcp::socket socket,
                                 on_http_request_callback_t on_http_request_callback);

private:
    session(boost::asio::ip::tcp::socket socket, on_http_request_callback_t on_http_request_callback);

    void run();

    void read();
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);

    void write(boost::beast::http::message_generator message);
    void on_write(boost::beast::error_code ec, const bool keep_alive);

    void close();

    boost::beast::tcp_stream stream_;
    boost::beast::flat_buffer buffer_;
    http_request_t request_;

    const on_http_request_callback_t on_http_request_callback_;
};

} // namespace ms::http
