#pragma once

#include "../types.hpp"

namespace instructions {

void process_set(const Instruction& instruction, int line_number, bool check_flag);
void process_print(const Instruction& instruction, int line_number, bool check_flag);
void process_move(const Instruction& instruction, int line_number, bool check_flag);
void process_wait(const Instruction& instruction, int line_number, bool check_flag);

void process_interactive_set(const Instruction& instruction);
void process_interactive_print(const Instruction& instruction);
void process_interactive_move(const Instruction& instruction);
void process_interactive_wait(const Instruction& instruction);

}  // namespace instructions
