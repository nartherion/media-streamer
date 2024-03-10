#include <ms/http/session.hpp>

#include <chrono>

#include <spdlog/spdlog.h>

namespace ms::http
{

void session::create_and_start(boost::asio::ip::tcp::socket socket,
                               on_http_request_callback_t on_http_request_callback)
{
    assert(on_http_request_callback);

    const auto s = std::shared_ptr<session>(new session(std::move(socket), std::move(on_http_request_callback)));
    s->run();
}

session::session(boost::asio::ip::tcp::socket socket, on_http_request_callback_t on_http_request_callback)
    : stream_(std::move(socket)),
      on_http_request_callback_(std::move(on_http_request_callback))
{}

void session::run()
{
    boost::asio::post(stream_.get_executor(), [self = shared_from_this()] { self->read(); });
}

void session::read()
{
    request_.clear();

    stream_.expires_after(std::chrono::seconds(30));

    boost::beast::http::async_read(stream_, buffer_, request_,
        [self = shared_from_this()](const boost::beast::error_code ec, const std::size_t bytes_transferred)
        {
            self->on_read(ec, bytes_transferred);
        });
}

void session::on_read(const boost::beast::error_code ec, [[maybe_unused]] const std::size_t bytes_transferred)
{
    if (ec)
    {
        if (ec == boost::beast::http::error::end_of_stream)
        {
            close();
        }
        else
        {
            SPDLOG_ERROR("Async read failed: {} ({})", ec.what(), ec.value());
        }

        return;
    }

    write(on_http_request_callback_(request_));
}

void session::write(boost::beast::http::message_generator message)
{
    const bool keep_alive = message.keep_alive();

    boost::beast::async_write(stream_, std::move(message),
        [self = shared_from_this(), keep_alive](const boost::beast::error_code &ec, const std::size_t)
        {
            self->on_write(ec, keep_alive);
        });
}

void session::on_write(boost::beast::error_code ec, const bool keep_alive)
{
    if (ec)
    {
        SPDLOG_ERROR("Async write failed: {} ({})", ec.what(), ec.value());
        return;
    }

    if (!keep_alive)
    {
        close();
    }

    read();
}

void session::close()
{
    boost::beast::error_code ec;
    stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    if (ec && ec != boost::beast::errc::not_connected)
    {
        SPDLOG_ERROR("Failed to close session: {} ({})", ec.what(), ec.value());
    }
}

} // namespace ms::http
