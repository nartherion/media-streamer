#pragma once

#include <memory>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/beast/core/error.hpp>

#include "session.hpp"

namespace ms::http
{

class server : public std::enable_shared_from_this<server>
{
public:
    static void create_and_start(boost::asio::io_context &context, boost::asio::ip::tcp::endpoint endpoint,
                                 const std::string &root);

private:
    server(boost::asio::io_context &context, boost::asio::ip::tcp::acceptor acceptor, const std::string &root);

    void accept();
    void on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);

    boost::beast::http::message_generator process_http_request(session::http_request_t request);

    boost::asio::io_context &context_;
    boost::asio::ip::tcp::acceptor acceptor_;

    const std::string root_;
};

} // namespace ms::http
