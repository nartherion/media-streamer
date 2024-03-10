#include <ms/http/client.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/beast/version.hpp>

#include <spdlog/spdlog.h>

int main(const int argc, const char ** const argv)
{
    if (argc < 3)
    {
        SPDLOG_ERROR("Bad arguments");
        return EXIT_FAILURE;
    }

    boost::asio::io_context context;

    const ms::http::client_config config
    {
        .host = argv[1],
        .port = argv[2],
        .target = argv[3],
        .version = 10
    };

    ms::http::client::create_and_start(context, config);
    context.run();

    return EXIT_SUCCESS;
}
