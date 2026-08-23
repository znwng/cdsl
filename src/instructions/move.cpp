#include "../../include/instructions/move.hpp"

#include <cstdio>
#include <iostream>
#include <string>

#include "../../include/diagnostics.hpp"
#include "../../include/expression.hpp"
#include "../../include/validation.hpp"
#include "../../include/variables.hpp"

namespace instructions {

namespace {

void move_function(const std::string& component_label, float value) {
    // Placeholder code
    // To be redefined for actual hardware.
    std::printf("Moved %s by %f\n\n", component_label.c_str(), value);
}

}  // namespace

void process_move(const Instruction& instruction, int line_number, bool check_flag) {
    if (instruction.size() != 3) {
        if (check_flag) {
            interpreter_error_continue(line_number, "Invalid number of arguments", instruction);
            return;
        }

        interpreter_error(line_number, "Invalid number of arguments", instruction);
        return;
    }

    const std::string& component_label = instruction[1];
    const std::string& value = instruction[2];

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
            float result = evaluate_expression(expression);

            if (!check_flag) {
                move_function(component_label, result);
            }
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
    if (value.starts_with('$')) {
        std::string variable_name = value.substr(1);

        if (!has_variable(variable_name)) {
            if (check_flag) {
                interpreter_error_continue(line_number, "Unknown variable: " + variable_name, instruction);
                return;
            }

            interpreter_error(line_number, "Unknown variable: " + variable_name, instruction);
            return;
        }

        if (!check_flag) {
            move_function(component_label, get_variable(variable_name));
        }

        return;
    }

    // Literal float
    auto successful_conversion = is_valid_float_value(value);

    if (!successful_conversion) {
        if (check_flag) {
            interpreter_error_continue(line_number, successful_conversion.error(), instruction);
            return;
        }

        interpreter_error(line_number, successful_conversion.error(), instruction);
        return;
    }

    if (!check_flag) {
        move_function(component_label, *successful_conversion);
    }
}

void process_interactive_move(const Instruction& instruction) {
    if (instruction.size() != 3) {
        std::cerr << "Invalid number of arguments. " << "Example: `MOVE COMPONENT_NAME VALUE`" << '\n';
        return;
    }

    const std::string& component_label = instruction[1];
    const std::string& value = instruction[2];

    // Expression
    if (value.starts_with("#[")) {
        if (value.size() < 3 || value.back() != ']') {
            std::cerr << "Invalid expression\n";
            return;
        }

        std::string expression = value.substr(2, value.size() - 3);

        try {
            float result = evaluate_expression(expression);

            move_function(component_label, result);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }

        return;
    }

    // Variable
    if (value.starts_with('$')) {
        std::string variable_name = value.substr(1);

        if (!has_variable(variable_name)) {
            std::cerr << "Unknown variable: " << variable_name << '\n';
            return;
        }

        move_function(component_label, get_variable(variable_name));
        return;
    }

    // Literal float
    auto successful_conversion = is_valid_float_value(value);

    if (!successful_conversion) {
        std::cerr << successful_conversion.error() << '\n';
        return;
    }

    move_function(component_label, *successful_conversion);
}

}  // namespace instructions
