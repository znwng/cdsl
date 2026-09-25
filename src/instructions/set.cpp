#include <iostream>
#include <string>

#include "core/diagnostics.hpp"
#include "core/expression.hpp"
#include "core/validation.hpp"
#include "core/variables.hpp"
#include "instructions/instruction.hpp"

void process_set(const Instruction& instruction) {
    if (instruction.size() != 3) {
        diagnostics::error("Invalid number of arguments. Example: `SET VARIABLE VALUE`", instruction);
        return;
    }

    if (!is_valid_variable_name(instruction[1])) {
        diagnostics::error("Invalid variable name: " + instruction[1], instruction);
        return;
    }

    const std::string& variable_key = instruction[1];
    const std::string& value = instruction[2];

    float variable_value;

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

        variable_value = *result;
    }

    // Variable
    else if (value.starts_with('$')) {
        const std::string VARIABLE_NAME = value.substr(1);

        if (!has_variable(VARIABLE_NAME)) {
            diagnostics::error("Unknown variable: " + VARIABLE_NAME, instruction);
            return;
        }

        variable_value = get_variable(VARIABLE_NAME);
    }

    // Literal float
    else {
        auto result = is_valid_float_value(value);

        if (!result) {
            diagnostics::error(result.error(), instruction);
            return;
        }

        variable_value = *result;
    }

    // Store variable
    set_variable(variable_key, variable_value);

    std::cout << "variable " << variable_key << " set to " << variable_value << '\n';
}
