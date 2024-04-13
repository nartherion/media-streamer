#include <ms/framework/mpd/time_utils.hpp>

#include <ctime>

#include <vector>
#include <array>
#include <sstream>

namespace
{

std::vector<std::string> split_to_strings(const std::string &s, const char delimiter)
{
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> strings;

    while (std::getline(ss, item, delimiter))
    {
        strings.push_back(item);
    }

    return strings;
}

std::vector<int> split_to_integers(const std::string &s, const char delimiter)
{
    std::stringstream ss(s);
    std::string item;
    std::vector<int> integers;

    while (std::getline(ss, item, delimiter))
    {
        integers.push_back(std::stoi(item));
    }

    return integers;
}

std::chrono::system_clock::duration resolve_utc_date_time(const std::string &date_time_string)
{
    if (date_time_string.empty())
    {
        return {};
    }

    std::time_t raw_time;
    time(&raw_time);

    std::tm *time_info = gmtime(&raw_time);

    const std::vector<std::string> date_time = split_to_strings(date_time_string, 'T');
    const std::vector<int> date_chunks = split_to_integers(date_time.at(0), '-');
    const std::vector<int> time_chunks = split_to_integers(date_time.at(1), ':');

    time_info->tm_year = date_chunks.at(0) - 1900;
    time_info->tm_mon = date_chunks.at(1) - 1;
    time_info->tm_mday = date_chunks.at(2);

    time_info->tm_hour = time_chunks.at(0);
    time_info->tm_min = time_chunks.at(1);
    time_info->tm_sec = time_chunks.at(2);

    return std::chrono::system_clock::from_time_t(mktime(time_info)).time_since_epoch();
}

std::chrono::system_clock::duration get_current_utc_time()
{
    return std::chrono::system_clock::now().time_since_epoch();
}

} // namespace

std::uint32_t get_current_time_in_seconds()
{
    return get_utc_time_in_seconds(get_current_utc_time());
}

std::uint32_t get_utc_time_in_seconds(const std::chrono::system_clock::duration time_from_epoch)
{
    return static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(time_from_epoch).count());
}

std::uint32_t get_utc_time_in_seconds(const std::string &date_time_string)
{
    return get_utc_time_in_seconds(resolve_utc_date_time(date_time_string));
}

std::uint32_t get_duration_in_seconds(const std::string &duration)
{
    if (duration.empty() || duration.substr(0, 2) != "PT")
        return {};

    const std::array designators { 'H', 'M', 'S' };
    size_t cursor = 2;

    const auto parse = [&cursor, &duration](const char designator) -> std::uint32_t
    {
        const std::size_t end = duration.find(designator, cursor);
        if (end != std::string::npos)
        {
            return static_cast<std::uint32_t>(std::stoi(duration.substr(cursor, end - std::exchange(cursor, end + 1))));
        }

        return {};
    };

    const std::uint32_t hours = parse(designators.at(0));
    const std::uint32_t minutes = parse(designators.at(1));
    const std::uint32_t seconds = parse(designators.at(2));

    return hours * 3600 + minutes * 60 + seconds;
}
