#include "core/diagnostics.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

#include "core/color.hpp"

void display_instruction(const Instruction& instruction) {
    for (const std::string& token : instruction) {
        std::cout << token << ' ';
    }

    std::cout << '\n';
}

void interpreter_error(const std::string& message, const Instruction& instruction) {
    std::cerr << Color::RED << message << Color::RESET << '\n';
    display_instruction(instruction);
}
