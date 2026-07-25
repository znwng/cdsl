#pragma once

#include <expected>
#include <string>

std::expected<float, std::string> is_valid_float_value(const std::string& value);
std::expected<int, std::string> is_valid_int_value(const std::string& value);
bool is_valid_variable_name(const std::string& name);
bool is_valid_instruction_file(const std::string& file_path);

