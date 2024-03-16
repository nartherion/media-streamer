#include <ms/http/client.hpp>

#include <chrono>

#include <boost/beast/version.hpp>

#include <spdlog/spdlog.h>

namespace ms::http
{

namespace
{

client::http_request_t make_http_request(const client_config &config)
{
    client::http_request_t request;

    const auto [endpoint, request_config] = config;
    const auto [target, method] = request_config;

    request.method(method);
    request.target(target);
    request.set(boost::beast::http::field::host, endpoint.host_);
    request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    return request;
}

} // namespace

void client::create_and_start(boost::asio::io_context &context, const client_config &config)
{
    const auto c = std::shared_ptr<client>(new client(context));
    c->run(config);
}

client::client(boost::asio::io_context &context)
    : resolver_(context),
      stream_(context)
{}

void client::run(const client_config &config)
{
    request_ = make_http_request(config);

    const auto [host, port] = config.endpoint_;
    resolver_.async_resolve(host, std::to_string(port),
        [self = shared_from_this()]
        (const boost::beast::error_code ec, const boost::asio::ip::tcp::resolver::results_type results)
        {
            self->on_resolve(ec, results);
        });
}

void client::on_resolve(const boost::beast::error_code ec, const boost::asio::ip::tcp::resolver::results_type results)
{
    if (ec)
    {
        SPDLOG_ERROR("Failed to resolve: {} ({})", ec.what(), ec.value());
        return;
    }

    stream_.expires_after(std::chrono::seconds(30));

    stream_.async_connect(results,
        [self = shared_from_this()]
        (const boost::beast::error_code connect_ec, const boost::asio::ip::tcp::resolver::results_type::endpoint_type)
        {
            self->on_connect(connect_ec);
        });
}

void client::on_connect(const boost::beast::error_code ec)
{
    if (ec)
    {
        SPDLOG_ERROR("Failed to connect: {} ({})", ec.what(), ec.value());
        return;
    }

    stream_.expires_after(std::chrono::seconds(30));

    boost::beast::http::async_write(stream_, request_,
        [self = shared_from_this()](const boost::beast::error_code write_ec, const std::size_t)
        {
            self->on_write(write_ec);
        });
}

void client::on_write(const boost::beast::error_code ec)
{
    if (ec)
    {
        SPDLOG_ERROR("Failed to write: {} ({})", ec.what(), ec.value());
        return;
    }

    boost::beast::http::async_read(stream_, buffer_, response_,
        [self = shared_from_this()](const boost::beast::error_code read_ec, const std::size_t)
        {
            self->on_read(read_ec);
        });
}

void client::on_read(const boost::beast::error_code ec)
{
    if (ec)
    {
        SPDLOG_ERROR("Failed to read: {} ({})", ec.what(), ec.value());
        return;
    }

    SPDLOG_INFO("Received response: {}", response_.body());

    boost::beast::error_code shutdown_ec;
    stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, shutdown_ec);
    if (shutdown_ec && shutdown_ec != boost::beast::errc::not_connected)
    {
        SPDLOG_ERROR("Failed to shutdown: {} ({})", ec.what(), ec.value());
    }
}

} // namespace ms::http
