#pragma once

#include "../types.hpp"

namespace instructions {

void process_wait(const Instruction& instruction, int line_number, bool check_flag);

void process_interactive_wait(const Instruction& instruction);

}  // namespace instructions
