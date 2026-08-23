#include "instructions/wait.hpp"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <string>
#include <thread>

#include "diagnostics.hpp"
#include "expression.hpp"
#include "validation.hpp"
#include "variables.hpp"

namespace instructions {

namespace {

void wait_function(int delay) {
    // Placeholder code
    // Actual delay implementation must eventually be handled by hardware.
    std::printf("Waiting for %d milliseconds\n\n", delay);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

}  // namespace

void process_wait(const Instruction& instruction, int line_number, bool check_flag) {
    if (instruction.size() != 2) {
        if (check_flag) {
            interpreter_error_continue(line_number, "Invalid number of arguments", instruction);
            return;
        }

        interpreter_error(line_number, "Invalid number of arguments", instruction);
        return;
    }

    const std::string& value = instruction[1];

    int delay;

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
            delay = static_cast<int>(result);
        } catch (const std::exception& e) {
            if (check_flag) {
                interpreter_error_continue(line_number, e.what(), instruction);
                return;
            }

            interpreter_error(line_number, e.what(), instruction);
            return;
        }
    }

    // Variable
    else if (value.starts_with('$')) {
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

        delay = static_cast<int>(get_variable(variable_name));
    }

    // Literal integer
    else {
        auto successful_conversion = is_valid_int_value(value);

        if (!successful_conversion) {
            if (check_flag) {
                interpreter_error_continue(line_number, successful_conversion.error(), instruction);
                return;
            }

            interpreter_error(line_number, successful_conversion.error(), instruction);
            return;
        }

        delay = *successful_conversion;
    }

    // Common validation
    if (delay < 0) {
        const std::string message = "Delay cannot be negative: " + std::to_string(delay);

        if (check_flag) {
            interpreter_error_continue(line_number, message, instruction);
            return;
        }

        interpreter_error(line_number, message, instruction);
        return;
    }

    if (!check_flag) {
        wait_function(delay);
    }
}

void process_interactive_wait(const Instruction& instruction) {
    if (instruction.size() != 2) {
        std::cerr << "Invalid number of arguments. "
                  << "Example: `WAIT DURATION_MS`" << '\n';
        return;
    }

    const std::string& value = instruction[1];

    int delay;

    // Expression
    if (value.starts_with("#[")) {
        if (value.size() < 3 || value.back() != ']') {
            std::cerr << "Invalid expression\n";
            return;
        }

        std::string expression = value.substr(2, value.size() - 3);

        try {
            float result = evaluate_expression(expression);
            delay = static_cast<int>(result);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return;
        }
    }

    // Variable
    else if (value.starts_with('$')) {
        std::string variable_name = value.substr(1);

        if (!has_variable(variable_name)) {
            std::cerr << "Unknown variable: " << variable_name << '\n';
            return;
        }

        delay = static_cast<int>(get_variable(variable_name));
    }

    // Literal integer
    else {
        auto successful_conversion = is_valid_int_value(value);

        if (!successful_conversion) {
            std::cerr << successful_conversion.error() << '\n';
            return;
        }

        delay = *successful_conversion;
    }

    // Common validation
    if (delay < 0) {
        std::cerr << "Delay cannot be negative: " << delay << '\n';
        return;
    }

    wait_function(delay);
}

}  // namespace instructions
