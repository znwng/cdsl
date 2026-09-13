#include "instructions/set.hpp"

#include <iostream>
#include <string>

#include "diagnostics.hpp"
#include "expression.hpp"
#include "validation.hpp"
#include "variables.hpp"

namespace instructions {

void process_set(const Instruction& instruction) {
    if (instruction.size() != 3) {
        interpreter_error("Invalid number of arguments. Example: `SET VARIABLE VALUE`", instruction);
        return;
    }

    if (!is_valid_variable_name(instruction[1])) {
        interpreter_error("Invalid variable name: " + instruction[1], instruction);
        return;
    }

    const std::string& variable_key = instruction[1];
    const std::string& value = instruction[2];

    float variable_value;

    // Expression
    if (value.starts_with("#[")) {
        if (value.size() < 3 || value.back() != ']') {
            interpreter_error("Invalid expression", instruction);
            return;
        }

        std::string expression = value.substr(2, value.size() - 3);

        try {
            variable_value = evaluate_expression(expression);
        } catch (const std::exception& e) {
            interpreter_error(e.what(), instruction);
            return;
        }
    }

    // Variable
    else if (value.starts_with('$')) {
        std::string variable_name = value.substr(1);

        if (!has_variable(variable_name)) {
            interpreter_error("Unknown variable: " + variable_name, instruction);
            return;
        }

        variable_value = get_variable(variable_name);
    }

    // Literal float
    else {
        auto successful_conversion = is_valid_float_value(value);

        if (!successful_conversion) {
            interpreter_error(successful_conversion.error(), instruction);
            return;
        }

        variable_value = *successful_conversion;
    }

    set_variable(variable_key, variable_value);

    std::cout << "variable " << variable_key << " set to " << variable_value << '\n';
}

}  // namespace instructions
