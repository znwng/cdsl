#include "../include/diagnostics.hpp"

#include <cstdio>
#include <iostream>
#include <string>

#include "../include/types.hpp"

void display_instruction(const Instruction& instruction_vector) {
    for (const std::string& token : instruction_vector) {
        std::cout << token << ' ';
    }
    std::cout << std::endl;
}

[[noreturn]]
void interpreter_error(int line_number, const std::string& message, const Instruction& instruction_vector) {
    std::cout << Color::RED << "Line " << line_number << '\n' << message << Color::RESET << '\n';
    display_instruction(instruction_vector);
    std::exit(EXIT_FAILURE);
}

void interpreter_error_continue(int line_number, const std::string& message, const Instruction& instruction_vector) {
    std::cout << Color::RED << "Line " << line_number << '\n' << message << Color::RESET << '\n';

    display_instruction(instruction_vector);
    std::cout << '\n';
}
