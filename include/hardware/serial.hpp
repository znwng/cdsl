#pragma once

#include <string>

namespace hardware {

void send_command(const std::string& component_label, float value);

}  // namespace hardware
