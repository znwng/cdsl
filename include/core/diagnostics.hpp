#pragma once

#include <string>

#include "types.hpp"

namespace diagnostics {

void display_instruction(const Instruction& instruction);

void error(const std::string& message, const Instruction& instruction);

}  // namespace diagnostics
