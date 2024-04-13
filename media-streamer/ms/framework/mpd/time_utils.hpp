#pragma once

#include <cstdint>

#include <chrono>
#include <string>

std::uint32_t get_current_time_in_seconds();
std::uint32_t get_utc_time_in_seconds(std::chrono::system_clock::duration time_from_epoch);
std::uint32_t get_utc_time_in_seconds(const std::string &date_time_string);
std::uint32_t get_duration_in_seconds(const std::string &duration);
