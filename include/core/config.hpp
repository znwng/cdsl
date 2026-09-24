#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <toml++/toml.hpp>

namespace config {

using Error = std::string;

std::expected<toml::table, Error> load_config();

std::expected<bool, Error> component_exists(const toml::table& config, const std::string& component_name);

std::expected<uint8_t, Error> component_id(const toml::table& config, const std::string& component_name);

std::expected<bool, Error> value_within_limits(const toml::table& config, const std::string& component_name,
                                               double value);

std::expected<std::string, Error> arduino_port(const toml::table& config);

std::expected<int, Error> arduino_baud_rate(const toml::table& config);

}  // namespace config
