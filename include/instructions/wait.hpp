#pragma once

#include "../types.hpp"

namespace instructions {

void process_wait(const Instruction& instruction, int line_number, bool check_flag);

}  // namespace instructions
