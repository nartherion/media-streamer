#include <ms/http/client.hpp>

#include <string>

#include <boost/asio/io_context.hpp>
#include <boost/beast/version.hpp>

#include <spdlog/spdlog.h>

int main(const int argc, const char ** const argv)
{
    if (argc < 5)
    {
        SPDLOG_ERROR("Bad arguments");
        return EXIT_FAILURE;
    }

    boost::asio::io_context context;

    const ms::http::client_config config
    {
        .endpoint_ =
        {
            .host_ = argv[1],
            .port_ = std::stoi(argv[2])
        },
        .request_ =
        {
            .target_ = argv[3],
            .method_ = static_cast<boost::beast::http::verb>(std::stoi(argv[4]))
        },
    };

    ms::http::client::create_and_start(context, config);
    context.run();

    return EXIT_SUCCESS;
}
