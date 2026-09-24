#include <iostream>
#include <string>

#include "core/diagnostics.hpp"
#include "core/expression.hpp"
#include "core/variables.hpp"
#include "instructions/instruction.hpp"

void process_print(const Instruction& instruction) {
    if (instruction.size() != 2) {
        diagnostics::error("Invalid number of arguments. Example: `PRINT VALUE`", instruction);
        return;
    }

    const std::string& argument = instruction[1];

    // Expression
    if (argument.starts_with("#[")) {
        if (argument.size() < 3 || argument.back() != ']') {
            diagnostics::error("Invalid expression", instruction);
            return;
        }

        const std::string EXPRESSION = argument.substr(2, argument.size() - 3);

        auto result = evaluate_expression(EXPRESSION);

        if (!result) {
            diagnostics::error(result.error(), instruction);
            return;
        }

        std::cout << *result << '\n';

        return;
    }

    // Variable
    std::string variable_key = argument;

    if (variable_key.starts_with('$')) {
        variable_key.erase(0, 1);
    }

    if (!has_variable(variable_key)) {
        diagnostics::error("No variable with name " + variable_key, instruction);
        return;
    }

    std::cout << get_variable(variable_key) << '\n';
}
