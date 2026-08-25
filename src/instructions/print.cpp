#include "instructions/print.hpp"

#include <iostream>
#include <string>

#include "diagnostics.hpp"
#include "expression.hpp"
#include "variables.hpp"

namespace instructions {

void process_print(const Instruction& instruction, int line_number, bool check_flag) {
    if (instruction.size() != 2) {
        if (check_flag) {
            interpreter_error_continue(line_number, "Invalid number of arguments", instruction);
            return;
        }

        interpreter_error(line_number, "Invalid number of arguments", instruction);
        return;
    }

    const std::string& argument = instruction[1];

    // Expression
    if (argument.starts_with("#[")) {
        if (argument.size() < 3 || argument.back() != ']') {
            if (check_flag) {
                interpreter_error_continue(line_number, "Invalid expression", instruction);
                return;
            }

            interpreter_error(line_number, "Invalid expression", instruction);
            return;
        }

        std::string expression = argument.substr(2, argument.size() - 3);

        try {
            float result = evaluate_expression(expression);
            std::cout << result << '\n';
        } catch (const std::exception& e) {
            if (check_flag) {
                interpreter_error_continue(line_number, e.what(), instruction);
                return;
            }

            interpreter_error(line_number, e.what(), instruction);
            return;
        }

        return;
    }

    // Variable
    std::string variable_key = argument;

    if (variable_key.starts_with('$')) {
        variable_key.erase(0, 1);
    }

    if (!has_variable(variable_key)) {
        const std::string message = "No variable with name " + variable_key;

        if (check_flag) {
            interpreter_error_continue(line_number, message, instruction);
            return;
        }

        interpreter_error(line_number, message, instruction);
        return;
    }

    std::cout << get_variable(variable_key) << '\n';
}

void process_interactive_print(const Instruction& instruction) {
    if (instruction.size() != 2) {
        std::cerr << "Invalid number of arguments. " << "Example: `PRINT value`" << '\n';
        return;
    }

    const std::string& value = instruction[1];

    // Expression
    if (value.starts_with("#[")) {
        if (value.size() < 3 || value.back() != ']') {
            std::cerr << "Invalid expression\n";
            return;
        }

        std::string expression = value.substr(2, value.size() - 3);

        try {
            std::cout << evaluate_expression(expression) << '\n';
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }

        return;
    }

    // Variable
    std::string variable_key = value;

    if (variable_key.starts_with('$')) {
        variable_key.erase(0, 1);
    }

    if (!has_variable(variable_key)) {
        std::cerr << "No variable with name " << variable_key << '\n';
        return;
    }

    std::cout << get_variable(variable_key) << '\n';
}

}  // namespace instructions
