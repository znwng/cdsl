#pragma once

#include "../types.hpp"

namespace instructions {

void process_set(const Instruction& instruction);

void process_print(const Instruction& instruction);

void process_move(const Instruction& instruction);

void process_wait(const Instruction& instruction);

}  // namespace instructions
