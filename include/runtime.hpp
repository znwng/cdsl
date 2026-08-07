#pragma once

#include <types.hpp>

void process_instruction(const Instruction& instruction, int line_number, bool check_flag);
void process_interactive_instruction(const Instruction& instruction);
void run_interactive_mode();
