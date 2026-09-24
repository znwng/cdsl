#pragma once

#include <expected>
#include <string>

namespace arduino {

using Error = std::string;

std::expected<void, Error> send_command(const std::string& component_label, float value);

}  // namespace arduino
