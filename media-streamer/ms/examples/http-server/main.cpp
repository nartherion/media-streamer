#include <ms/http/server.hpp>

#include <boost/asio/io_context.hpp>

#include <spdlog/spdlog.h>

int main(const int argc, const char ** const argv)
{
    if (argc < 4)
    {
        SPDLOG_ERROR("Bad arguments");
        return EXIT_FAILURE;
    }

    boost::asio::io_context context;

    const boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(argv[1]),
                                            static_cast<boost::asio::ip::port_type>(std::stoi(argv[2])));

    ms::http::server::create_and_start(context, ep, argv[3]);
    context.run();

    return EXIT_SUCCESS;
}
