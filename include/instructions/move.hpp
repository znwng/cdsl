#pragma once

#include "../types.hpp"

namespace instructions {

void process_move(const Instruction& instruction, int line_number, bool check_flag);

void process_interactive_move(const Instruction& instruction);

}  // namespace instructions
