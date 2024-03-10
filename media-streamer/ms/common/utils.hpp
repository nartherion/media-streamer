#pragma once

#include <string>

namespace ms::common
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

[[nodiscard]] inline std::string file_type(const std::string_view path)
{
    const auto position = path.find(".");
    const std::string_view extension = (position == std::string_view::npos) ? "" : path.substr(position);

    if (extension == ".html")
    {
        return "text/html";
    }

    if (extension == ".jpeg")
    {
        return "image/jpeg";
    }

    return "application/text";
}

} // namespace media_streamer::http::utils
