#include "instructions/wait.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "diagnostics.hpp"
#include "expression.hpp"
#include "validation.hpp"
#include "variables.hpp"

void wait_function(int delay) {
    // Placeholder code.
    // Actual delay implementation must eventually be handled by hardware.
    std::cout << "Waiting for " << delay << " milliseconds\n\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void process_wait(const Instruction& instruction) {
    if (instruction.size() != 2) {
        interpreter_error("Invalid number of arguments. Example: `WAIT DURATION_MS`", instruction);
        return;
    }

    const std::string& value = instruction[1];

    int delay;

    // Expression
    if (value.starts_with("#[")) {
        if (value.size() < 3 || value.back() != ']') {
            interpreter_error("Invalid expression", instruction);
            return;
        }

        std::string expression = value.substr(2, value.size() - 3);

        try {
            float result = evaluate_expression(expression);
            delay = static_cast<int>(result);
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

        delay = static_cast<int>(get_variable(variable_name));
    }

    // Literal integer
    else {
        auto successful_conversion = is_valid_int_value(value);

        if (!successful_conversion) {
            interpreter_error(successful_conversion.error(), instruction);
            return;
        }

        delay = *successful_conversion;
    }

    // Validate delay
    if (delay < 0) {
        interpreter_error("Delay cannot be negative: " + std::to_string(delay), instruction);
        return;
    }

    wait_function(delay);
}
