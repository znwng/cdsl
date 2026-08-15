#pragma once

#include <string>

#include "../include/types.hpp"

void process_instructions_file(const std::string& file_path, bool check_flag);

void validate_without_executing(const std::string& file_path);

Instruction tokenize(const std::string& line);