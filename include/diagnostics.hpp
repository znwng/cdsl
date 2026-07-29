#pragma once

#include <string>
#include <vector>

namespace Color {

    inline constexpr const char* RESET = "\033[0m";
    inline constexpr const char* RED = "\033[31m";
    inline constexpr const char* GREEN = "\033[32m";
    inline constexpr const char* YELLOW = "\033[33m";
    inline constexpr const char* BLUE = "\033[34m";
    inline constexpr const char* CYAN = "\033[36m";

} // namespace Color

[[noreturn]]
void interpreter_error(int line_number, const std::string& message,
                       const std::vector<std::string>& instruction);

void interpreter_error_continue(const std::string& message,
                                const std::vector<std::string>& instruction);

void display_instruction(const std::vector<std::string>& instruction);

