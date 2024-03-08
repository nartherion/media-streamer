#pragma once

#include <string>

#include <boost/beast/core/string.hpp>

namespace ms::http::utils
{

[[nodiscard]] inline std::string concatenate_path(const std::string &base, const std::string &path)
{
    if (base.empty())
    {
        return path;
    }

    constexpr char path_separator = '/';

    if (base.back() == path_separator && path.front() == path_separator)
    {
        return base.substr(0, base.size() - 1) + path;
    }

    return base + path;
}

[[nodiscard]] inline std::string file_type(const boost::beast::string_view path)
{
    const std::string extension = [&path]
    {
        const auto position = path.find(".");

        if (position == boost::beast::string_view::npos)
        {
            return boost::beast::string_view();
        }

        return path.substr(position);
    }();

    if (extension == ".html")
    {
        return "text/html";
    }

    if (extension == "jpeg")
    {
        return "image/jpeg";
    }

    return "application/text";
}

} // namespace media_streamer::http::utils
