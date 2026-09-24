#pragma once

#include <expected>
#include <string>

std::expected<float, std::string> evaluate_expression(const std::string& expression);
