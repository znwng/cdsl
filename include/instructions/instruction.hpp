#pragma once

#include "core/types.hpp"
#include "toml++/toml.hpp"

void process_set(const Instruction& instruction);

void process_print(const Instruction& instruction);

void process_move(const toml::table& config, const Instruction& instruction);

void process_wait(const Instruction& instruction);
