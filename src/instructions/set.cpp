#include "instructions/set.hpp"

#include <iostream>
#include <string>

#include "diagnostics.hpp"
#include "expression.hpp"
#include "validation.hpp"
#include "variables.hpp"

namespace instructions {

void process_set(const Instruction& instruction, int line_number, bool check_flag) {
    if (instruction.size() != 3) {
        if (check_flag) {
            interpreter_error_continue(line_number, "Invalid number of arguments", instruction);
            return;
        }

        interpreter_error(line_number, "Invalid number of arguments", instruction);
        return;
    }

    if (!is_valid_variable_name(instruction[1])) {
        const std::string message = "Invalid variable name: " + instruction[1];

        if (check_flag) {
            interpreter_error_continue(line_number, message, instruction);
            return;
        }

        interpreter_error(line_number, message, instruction);
        return;
    }

    const std::string& variable_key = instruction[1];
    const std::string& value = instruction[2];

    float variable_value;

    // Expression
    if (value.starts_with("#[")) {
        if (value.size() < 3 || value.back() != ']') {
            if (check_flag) {
                interpreter_error_continue(line_number, "Invalid expression", instruction);
                return;
            }

            interpreter_error(line_number, "Invalid expression", instruction);
            return;
        }

        std::string expression = value.substr(2, value.size() - 3);

        try {
            variable_value = evaluate_expression(expression);
        } catch (const std::exception& e) {
            if (check_flag) {
                interpreter_error_continue(line_number, e.what(), instruction);
                return;
            }

            interpreter_error(line_number, e.what(), instruction);
            return;
        }
    }

    // Variable
    else if (value.starts_with('$')) {
        std::string variable_name = value.substr(1);

        if (!has_variable(variable_name)) {
            const std::string message = "Unknown variable: " + variable_name;

            if (check_flag) {
                interpreter_error_continue(line_number, message, instruction);
                return;
            }

            interpreter_error(line_number, message, instruction);
            return;
        }

        variable_value = get_variable(variable_name);
    }

    // Literal float
    else {
        auto successful_conversion = is_valid_float_value(value);

        if (!successful_conversion) {
            if (check_flag) {
                interpreter_error_continue(line_number, successful_conversion.error(), instruction);
                return;
            }

            interpreter_error(line_number, successful_conversion.error(), instruction);
            return;
        }

        variable_value = *successful_conversion;
    }

    set_variable(variable_key, variable_value);

    if (check_flag) {
        std::cout << "variable " << variable_key << " set to " << variable_value << '\n';
    }
}

void process_interactive_set(const Instruction& instruction) {
    if (instruction.size() != 3) {
        std::cerr << "Invalid number of arguments. " << "Example: `SET variable_name value`" << '\n';
        return;
    }

    if (!is_valid_variable_name(instruction[1])) {
        std::cerr << "Invalid variable name: " << instruction[1] << '\n';
        return;
    }

    const std::string& variable_key = instruction[1];
    const std::string& value = instruction[2];

    float variable_value;

    // Expression
    if (value.starts_with("#[")) {
        if (value.size() < 3 || value.back() != ']') {
            std::cerr << "Invalid expression\n";
            return;
        }

        std::string expression = value.substr(2, value.size() - 3);

        try {
            variable_value = evaluate_expression(expression);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return;
        }
    }

    // Variable
    else if (value.starts_with('$')) {
        std::string variable_name = value.substr(1);

        if (!has_variable(variable_name)) {
            std::cerr << "Unknown variable: " << variable_name << '\n';
            return;
        }

        variable_value = get_variable(variable_name);
    }

    // Literal float
    else {
        auto successful_conversion = is_valid_float_value(value);

        if (!successful_conversion) {
            std::cerr << successful_conversion.error() << '\n';
            return;
        }

        variable_value = *successful_conversion;
    }

    set_variable(variable_key, variable_value);

    std::cout << "variable " << variable_key << " set to " << variable_value << '\n';
}

}  // namespace instructions
