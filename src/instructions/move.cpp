#include "instructions/move.hpp"

#include <cstdio>
#include <string>

#include "config.hpp"
#include "diagnostics.hpp"
#include "expression.hpp"
#include "hardware/serial.hpp"
#include "validation.hpp"
#include "variables.hpp"

namespace instructions {

namespace {

void move_function(const std::string& component_label, float value) {
    hardware::send_command(component_label, value);
    std::printf("Moved %s by %f\n\n", component_label.c_str(), value);
}

bool validate_move_value(const std::string& component_label, float value) {
    if (!config::value_within_limits(component_label, value)) {
        return false;
    }

    return true;
}

}  // namespace

void process_move(const Instruction& instruction) {
    if (instruction.size() != 3) {
        interpreter_error(
            "Invalid number of arguments. "
            "Example: `MOVE COMPONENT_NAME VALUE`",
            instruction);
        return;
    }

    const std::string& component_label = instruction[1];

    if (!config::component_exists(component_label)) {
        interpreter_error("Undefined component: " + component_label, instruction);
        return;
    }

    const std::string& value = instruction[2];

    // Expression
    if (value.starts_with("#[")) {
        if (value.size() < 3 || value.back() != ']') {
            interpreter_error("Invalid expression", instruction);
            return;
        }

        std::string expression = value.substr(2, value.size() - 3);

        try {
            float result = evaluate_expression(expression);

            if (!validate_move_value(component_label, result)) {
                interpreter_error("Value out of range for " + component_label + ": " + std::to_string(result),
                                  instruction);
                return;
            }

            move_function(component_label, result);
        } catch (const std::exception& e) {
            interpreter_error(e.what(), instruction);
        }

        return;
    }

    // Variable
    if (value.starts_with('$')) {
        std::string variable_name = value.substr(1);

        if (!has_variable(variable_name)) {
            interpreter_error("Unknown variable: " + variable_name, instruction);
            return;
        }

        float result = get_variable(variable_name);

        if (!validate_move_value(component_label, result)) {
            interpreter_error("Value out of range for " + component_label + ": " + std::to_string(result), instruction);
            return;
        }

        move_function(component_label, result);
        return;
    }

    // Literal float
    auto successful_conversion = is_valid_float_value(value);

    if (!successful_conversion) {
        interpreter_error(successful_conversion.error(), instruction);
        return;
    }

    if (!validate_move_value(component_label, *successful_conversion)) {
        interpreter_error("Value out of range for " + component_label + ": " + value, instruction);
        return;
    }

    move_function(component_label, *successful_conversion);
}

}  // namespace instructions
