#include <ms/http/server.hpp>
#include <ms/common/utils.hpp>

#include <spdlog/spdlog.h>

#include <boost/asio/strand.hpp>
#include <boost/beast/version.hpp>

namespace ms::http
{

namespace
{

boost::beast::http::message_generator make_html_response(const session::http_request_t &request,
                                                         const std::string_view body)
{
    boost::beast::http::response<boost::beast::http::string_body> response(boost::beast::http::status::bad_request,
                                                                           request.version());

    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "text/html");
    response.keep_alive(request.keep_alive());
    response.body() = body;
    response.prepare_payload();

    return response;
}

boost::beast::http::message_generator make_bad_request_response(const session::http_request_t &request,
                                                                const std::string_view reason)
{
    return make_html_response(request, fmt::format("Bad request: {}", reason));
}

boost::beast::http::message_generator make_not_found_response(const session::http_request_t &request,
                                                              const std::string_view target)
{
    return make_html_response(request, fmt::format("The resource '{}' was not found", target));
}

boost::beast::http::message_generator make_server_error_message(const session::http_request_t &request,
                                                                const std::string_view what)
{
    return make_html_response(request, fmt::format("An error occured: {}", what));
}

} // namespace

void server::create_and_start(boost::asio::io_context &context, const boost::asio::ip::tcp::endpoint endpoint,
                              const std::string &root)
{
    boost::asio::ip::tcp::acceptor acceptor(boost::asio::make_strand(context));

    boost::beast::error_code ec;
    acceptor.open(endpoint.protocol(), ec);
    if (ec)
    {
        SPDLOG_ERROR("Failed to open acceptor: {} ({})", ec.what(), ec.value());
        return;
    }

    acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
    if (ec)
    {
        SPDLOG_ERROR("Failed to configure acceptor: {} ({})", ec.what(), ec.value());
        return;
    }

    acceptor.bind(endpoint, ec);
    if (ec)
    {
        SPDLOG_ERROR("Failed to bind acceptor to endpoint: {} ({})", ec.what(), ec.value());
        return;
    }

    acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec)
    {
        SPDLOG_ERROR("Failed to listen for connections: {} ({})", ec.what(), ec.value());
        return;
    }

    const auto s = std::shared_ptr<server>(new server(context, std::move(acceptor), root));
    s->accept();
}

server::server(boost::asio::io_context &context, boost::asio::ip::tcp::acceptor acceptor, const std::string &root)
    : root_(root),
      context_(context),
      acceptor_(std::move(acceptor))
{}

void server::accept()
{
    acceptor_.async_accept(boost::asio::make_strand(context_),
        [self = shared_from_this()](const boost::beast::error_code ec, boost::asio::ip::tcp::socket socket)
        {
            self->on_accept(ec, std::move(socket));
        });
}

void server::on_accept(const boost::beast::error_code ec, boost::asio::ip::tcp::socket socket)
{
    if (ec)
    {
        SPDLOG_ERROR("Failed to accept the connection: {} ({})", ec.what(), ec.value());
        return;
    }

    session::create_and_start(std::move(socket),
        [self = shared_from_this()](session::http_request_t request)
        {
            return self->process_http_request(std::move(request));
        });

    accept();
}

boost::beast::http::message_generator server::process_http_request(session::http_request_t request)
{
    const boost::beast::http::verb method = request.method();

    if (method != boost::beast::http::verb::get && method != boost::beast::http::verb::head)
    {
        return make_bad_request_response(request, "HTTP method not supported");
    }

    const std::string &target = request.target();
    if (target.empty() || target[0] != '/' || target.find("..") != boost::beast::string_view::npos)
    {
        return make_bad_request_response(request, "Bad request target");
    }

    const std::string path = [this, &target]
    {
        const std::string concatenated_path = common::concatenate_path(root_, target);

        if (concatenated_path.back() == '/')
            return concatenated_path + "index.html";

        return concatenated_path;
    }();

    boost::beast::error_code ec;
    boost::beast::http::file_body::value_type body;
    body.open(path.c_str(), boost::beast::file_mode::scan, ec);

    if (ec == boost::beast::errc::no_such_file_or_directory)
    {
        return make_not_found_response(request, target);
    }

    if (ec)
    {
        return make_server_error_message(request, ec.message());
    }

    const std::size_t size = body.size();
    const unsigned &version = request.version();

    if (method == boost::beast::http::verb::head)
    {
        boost::beast::http::response<boost::beast::http::empty_body> response(boost::beast::http::status::ok, version);

        response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        response.set(boost::beast::http::field::content_type, common::file_type(path));
        response.content_length(size);
        response.keep_alive(request.keep_alive());

        return response;
    }

    auto body_tuple = std::make_tuple(std::move(body));
    const auto status_tuple = std::make_tuple(boost::beast::http::status::ok, version);

    boost::beast::http::response<boost::beast::http::file_body> response(std::piecewise_construct,
                                                                         std::move(body_tuple), status_tuple);

    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, common::file_type(path));
    response.content_length(size);
    response.keep_alive(request.keep_alive());

    return response;
}

} // namespace ms::http
