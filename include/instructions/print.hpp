#pragma once

#include "../types.hpp"

namespace instructions {

void process_print(const Instruction& instruction, int line_number, bool check_flag);
void process_interactive_print(const Instruction& instruction);

}  // namespace instructions
