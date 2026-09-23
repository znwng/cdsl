#pragma once

#include "core/types.hpp"

void process_set(const Instruction& instruction);

void process_print(const Instruction& instruction);

void process_move(const Instruction& instruction);

void process_wait(const Instruction& instruction);
