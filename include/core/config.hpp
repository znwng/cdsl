#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <toml++/toml.hpp>

namespace config {

using Error = std::string;

std::expected<void, Error> check_and_create_config_dir();

std::expected<toml::table, Error> load_config();

std::expected<bool, Error> component_exists(const std::string& component_name);

std::expected<uint8_t, Error> component_id(const std::string& component_name);

std::expected<bool, Error> value_within_limits(const std::string& component_name, double value);

std::expected<std::string, Error> arduino_port();

std::expected<int, Error> arduino_baud_rate();

}  // namespace config
