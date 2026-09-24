#include <iostream>
#include <string>

#include "core/config.hpp"
#include "core/diagnostics.hpp"
#include "core/expression.hpp"
#include "core/validation.hpp"
#include "core/variables.hpp"
#include "hardware/arduino.hpp"
#include "instructions/instruction.hpp"

void process_move(const Instruction& instruction) {
    if (instruction.size() != 3) {
        diagnostics::error(
            "Invalid number of arguments. "
            "Example: `MOVE COMPONENT_NAME VALUE`",
            instruction);
        return;
    }

    const std::string& component_label = instruction[1];

    // Check component
    auto component_exists = config::component_exists(component_label);

    if (!component_exists) {
        diagnostics::error(component_exists.error(), instruction);
        return;
    }

    if (!*component_exists) {
        diagnostics::error("Undefined component: " + component_label, instruction);
        return;
    }

    const std::string& value = instruction[2];

    // Expression
    if (value.starts_with("#[")) {
        if (value.size() < 3 || value.back() != ']') {
            diagnostics::error("Invalid expression", instruction);
            return;
        }

        const std::string EXPRESSION = value.substr(2, value.size() - 3);

        auto result = evaluate_expression(EXPRESSION);

        if (!result) {
            diagnostics::error(result.error(), instruction);
            return;
        }

        auto within_limits = config::value_within_limits(component_label, *result);

        if (!within_limits) {
            diagnostics::error(within_limits.error(), instruction);
            return;
        }

        if (!*within_limits) {
            diagnostics::error("Value out of range for " + component_label + ": " + std::to_string(*result),
                               instruction);
            return;
        }

        auto sent = arduino::send_command(component_label, *result);

        if (!sent) {
            diagnostics::error(sent.error(), instruction);
            return;
        }

        std::cout << "Moved " << component_label << " by " << value << "\n\n";

        return;
    }

    // Variable
    if (value.starts_with('$')) {
        const std::string VARIABLE_NAME = value.substr(1);

        if (!has_variable(VARIABLE_NAME)) {
            diagnostics::error("Unknown variable: " + VARIABLE_NAME, instruction);
            return;
        }

        const float RESULT = get_variable(VARIABLE_NAME);

        auto within_limits = config::value_within_limits(component_label, RESULT);

        if (!within_limits) {
            diagnostics::error(within_limits.error(), instruction);
            return;
        }

        if (!*within_limits) {
            diagnostics::error("Value out of range for " + component_label + ": " + std::to_string(RESULT),
                               instruction);
            return;
        }

        auto sent = arduino::send_command(component_label, RESULT);

        if (!sent) {
            diagnostics::error(sent.error(), instruction);
            return;
        }

        std::cout << "Moved " << component_label << " by " << value << "\n\n";

        return;
    }

    // Literal float
    auto result = is_valid_float_value(value);

    if (!result) {
        diagnostics::error(result.error(), instruction);
        return;
    }

    auto within_limits = config::value_within_limits(component_label, *result);

    if (!within_limits) {
        diagnostics::error(within_limits.error(), instruction);
        return;
    }

    if (!*within_limits) {
        diagnostics::error("Value out of range for " + component_label + ": " + value, instruction);
        return;
    }

    auto sent = arduino::send_command(component_label, *result);

    if (!sent) {
        diagnostics::error(sent.error(), instruction);
        return;
    }

    std::cout << "Moved " << component_label << " by " << value << "\n\n";
}
