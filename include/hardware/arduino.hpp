#pragma once

#include <expected>
#include <string>

#include "toml++/toml.hpp"

namespace arduino {

using Error = std::string;

std::expected<void, Error> send_command(const toml::table& config, const std::string& component_label, float value);

}  // namespace arduino
