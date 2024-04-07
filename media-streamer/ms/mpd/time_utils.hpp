#pragma once

#include <cstdint>

#include <chrono>
#include <string>

[[nodiscard]] std::uint32_t get_current_time_in_seconds();
[[nodiscard]] std::uint32_t get_utc_time_in_seconds(const std::chrono::system_clock::duration time_from_epoch);
[[nodiscard]] std::uint32_t get_utc_time_in_seconds(const std::string &date_time_string);
[[nodiscard]] std::uint32_t get_duration_in_seconds(const std::string &duration);
