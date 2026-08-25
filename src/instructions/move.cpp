#include "instructions/move.hpp"

#include <cstdio>
#include <iostream>
#include <string>

#include "config.hpp"
#include "diagnostics.hpp"
#include "expression.hpp"
#include "validation.hpp"
#include "variables.hpp"

namespace instructions {

namespace {

void move_function(const std::string& component_label, float value) {
    // Placeholder code
    // To be redefined for actual hardware.
    std::printf("Moved %s by %f\n\n", component_label.c_str(), value);
}

bool validate_move_value(const std::string& component_label, float value) {
    if (!config::value_within_limits(component_label, value)) {
        std::cerr << "Value out of range for " << component_label << ": " << value << '\n';

        return false;
    }

    return true;
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

    if (!config::component_exists(component_label)) {
        const std::string message = "Undefined component: " + component_label;

        if (check_flag) {
            interpreter_error_continue(line_number, message, instruction);
            return;
        }

        interpreter_error(line_number, message, instruction);
        return;
    }

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

            if (!config::value_within_limits(component_label, result)) {
                const std::string message = "Value out of range: " + std::to_string(result);

                if (check_flag) {
                    interpreter_error_continue(line_number, message, instruction);
                    return;
                }

                interpreter_error(line_number, message, instruction);
                return;
            }

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
            const std::string message = "Unknown variable: " + variable_name;

            if (check_flag) {
                interpreter_error_continue(line_number, message, instruction);
                return;
            }

            interpreter_error(line_number, message, instruction);
            return;
        }

        float result = get_variable(variable_name);

        if (!config::value_within_limits(component_label, result)) {
            const std::string message = "Value out of range: " + std::to_string(result);

            if (check_flag) {
                interpreter_error_continue(line_number, message, instruction);
                return;
            }

            interpreter_error(line_number, message, instruction);
            return;
        }

        if (!check_flag) {
            move_function(component_label, result);
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

    if (!config::value_within_limits(component_label, *successful_conversion)) {
        const std::string message = "Value out of range: " + value;

        if (check_flag) {
            interpreter_error_continue(line_number, message, instruction);
            return;
        }

        interpreter_error(line_number, message, instruction);
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

    if (!config::component_exists(component_label)) {
        std::cerr << "Undefined component: " << component_label << '\n';
        return;
    }

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

            if (!validate_move_value(component_label, result)) {
                return;
            }

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

        float result = get_variable(variable_name);

        if (!validate_move_value(component_label, result)) {
            return;
        }

        move_function(component_label, result);
        return;
    }

    // Literal float
    auto successful_conversion = is_valid_float_value(value);

    if (!successful_conversion) {
        std::cerr << successful_conversion.error() << '\n';
        return;
    }

    if (!validate_move_value(component_label, *successful_conversion)) {
        return;
    }

    move_function(component_label, *successful_conversion);
}

}  // namespace instructions
