#pragma once

#include <string>
#include <toml++/toml.hpp>

namespace config {

void check_and_create_config_dir();

toml::table load_config();

bool component_exists(const std::string& component_name);

uint8_t component_id(const std::string& component_name);

bool value_within_limits(const std::string& component_name, double value);

std::string arduino_port();

int arduino_baud_rate();

}  // namespace config
