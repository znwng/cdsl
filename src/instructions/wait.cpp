#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "core/diagnostics.hpp"
#include "core/expression.hpp"
#include "core/validation.hpp"
#include "core/variables.hpp"
#include "instructions/instruction.hpp"

void wait_function(int delay) {
    // Placeholder code.
    // Actual delay implementation must eventually be handled by hardware.
    std::cout << "Waiting for " << delay << " milliseconds\n\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void process_wait(const Instruction& instruction) {
    if (instruction.size() != 2) {
        diagnostics::error("Invalid number of arguments. Example: `WAIT DURATION_MS`", instruction);
        return;
    }

    const std::string& value = instruction[1];

    int delay;

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

        delay = static_cast<int>(*result);
    }

    // Variable
    else if (value.starts_with('$')) {
        const std::string VARIABLE_NAME = value.substr(1);

        if (!has_variable(VARIABLE_NAME)) {
            diagnostics::error("Unknown variable: " + VARIABLE_NAME, instruction);
            return;
        }

        delay = static_cast<int>(get_variable(VARIABLE_NAME));
    }

    // Literal integer
    else {
        auto result = is_valid_int_value(value);

        if (!result) {
            diagnostics::error(result.error(), instruction);
            return;
        }

        delay = *result;
    }

    // Validate delay
    if (delay < 0) {
        diagnostics::error("Delay cannot be negative: " + std::to_string(delay), instruction);
        return;
    }

    wait_function(delay);
}
