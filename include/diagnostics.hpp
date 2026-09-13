#pragma once

#include <string>

#include "types.hpp"

void display_instruction(const Instruction& instruction);

void interpreter_error(const std::string& message, const Instruction& instruction);
