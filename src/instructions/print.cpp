#include "instructions/print.hpp"

#include <iostream>
#include <string>

#include "diagnostics.hpp"
#include "expression.hpp"
#include "variables.hpp"

namespace instructions {

void process_print(const Instruction& instruction) {
    if (instruction.size() != 2) {
        interpreter_error("Invalid number of arguments. Example: `PRINT VALUE`", instruction);
        return;
    }

    const std::string& argument = instruction[1];

    // Expression
    if (argument.starts_with("#[")) {
        if (argument.size() < 3 || argument.back() != ']') {
            interpreter_error("Invalid expression", instruction);
            return;
        }

        std::string expression = argument.substr(2, argument.size() - 3);

        try {
            float result = evaluate_expression(expression);
            std::cout << result << '\n';
        } catch (const std::exception& e) {
            interpreter_error(e.what(), instruction);
        }

        return;
    }

    // Variable
    std::string variable_key = argument;

    if (variable_key.starts_with('$')) {
        variable_key.erase(0, 1);
    }

    if (!has_variable(variable_key)) {
        interpreter_error("No variable with name " + variable_key, instruction);
        return;
    }

    std::cout << get_variable(variable_key) << '\n';
}

}  // namespace instructions
