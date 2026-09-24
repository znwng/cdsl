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

    if (!config::component_exists(component_label)) {
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

        std::string expression = value.substr(2, value.size() - 3);

        try {
            float result = evaluate_expression(expression);

            if (!config::value_within_limits(component_label, result)) {
                diagnostics::error("Value out of range for " + component_label + ": " + std::to_string(result),
                                   instruction);
                return;
            }

            arduino::send_command(component_label, result);
            std::cout << "Moved " << component_label << " by " << value << "\n\n";

        } catch (const std::exception& e) {
            diagnostics::error(e.what(), instruction);
        }

        return;
    }

    // Variable
    if (value.starts_with('$')) {
        std::string variable_name = value.substr(1);

        if (!has_variable(variable_name)) {
            diagnostics::error("Unknown variable: " + variable_name, instruction);
            return;
        }

        float result = get_variable(variable_name);

        if (!config::value_within_limits(component_label, result)) {
            diagnostics::error("Value out of range for " + component_label + ": " + std::to_string(result),
                               instruction);
            return;
        }

        arduino::send_command(component_label, result);
        std::cout << "Moved " << component_label << " by " << value << "\n\n";

        return;
    }

    // Literal float
    auto successful_conversion = is_valid_float_value(value);

    if (!successful_conversion) {
        diagnostics::error(successful_conversion.error(), instruction);
        return;
    }

    if (!config::value_within_limits(component_label, *successful_conversion)) {
        diagnostics::error("Value out of range for " + component_label + ": " + value, instruction);
        return;
    }

    arduino::send_command(component_label, *successful_conversion);
    std::cout << "Moved " << component_label << " by " << value << "\n\n";
}
