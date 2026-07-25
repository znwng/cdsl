#include "../include/runtime.hpp"

#include "../include/constants.hpp"
#include "../include/diagnostics.hpp"
#include "../include/validation.hpp"

#include <chrono>
#include <cstdio>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace {

    enum class INSTRUCTION_SET { SET, MOVE, WAIT, INVALID };

    void move_function(const std::string& component_label, float value) {
        // Placeholder code
        // To be redefined
        std::printf("Moved %s by %f\n\n", component_label.c_str(), value);
    }

    void wait_function(int delay) {
        // Placeholder code
        // To be redefined

        // Actual delay implementation must be done on the hardware
        std::printf("Waiting for %d milliseconds\n\n", delay);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }

} // namespace

void execute_instruction(const std::vector<std::string>& instruction_vector, int line_number) {
    std::string action = instruction_vector[0];

    static const std::unordered_map<std::string, INSTRUCTION_SET> opcode_table {
        {"SET", INSTRUCTION_SET::SET},
        {"MOVE", INSTRUCTION_SET::MOVE},
        {"WAIT", INSTRUCTION_SET::WAIT},
    };

    INSTRUCTION_SET opcode;

    auto it = opcode_table.find(action);
    if (it == opcode_table.end()) {
        opcode = INSTRUCTION_SET::INVALID;
    } else {
        opcode = it->second;
    }

    switch (opcode) {
        case INSTRUCTION_SET::SET: {
            if (instruction_vector.size() != 3) {
                interpreter_error(line_number, "Invalid number of arguments", instruction_vector);
            }

            if (!is_valid_variable_name(instruction_vector[1])) {
                interpreter_error(line_number, "Invalid constant name: " + instruction_vector[1],
                                  instruction_vector);
            }
            std::string constant_key = instruction_vector[1];
            auto successful_conversion = is_valid_float_value(instruction_vector[2]);

            if (!successful_conversion) {
                interpreter_error(line_number, successful_conversion.error(), instruction_vector);
            }

            float constant_value = *successful_conversion;
            set_constant(constant_key, constant_value);
            break;
        }

        case INSTRUCTION_SET::MOVE: {
            if (instruction_vector.size() != 3) {
                interpreter_error(line_number, "Invalid number of arguments", instruction_vector);
            }
            std::string component_label = instruction_vector[1];
            std::string value = instruction_vector[2];
            if (value[0] == '$') {
                value.erase(0, 1);

                if (!has_constant(value)) {
                    interpreter_error(line_number, "Unknown constant: " + value,
                                      instruction_vector);
                }

                move_function(component_label, get_constant(value));
                break;
            }

            auto successful_conversion = is_valid_float_value(value);
            if (!successful_conversion) {
                interpreter_error(line_number, successful_conversion.error(), instruction_vector);
            }

            move_function(component_label, *successful_conversion);
            break;
        }

        case INSTRUCTION_SET::WAIT: {
            if (instruction_vector.size() != 2) {
                interpreter_error(line_number, "Invalid number of arguments", instruction_vector);
            }

            auto successful_conversion = is_valid_int_value(instruction_vector[1]);

            if (!successful_conversion) {
                interpreter_error(line_number, successful_conversion.error(), instruction_vector);
            }

            int delay = *successful_conversion;
            if (delay < 0) {
                interpreter_error(line_number, "Delay cannot be negative " + std::to_string(delay),
                                  instruction_vector);
            }

            wait_function(delay);
            break;
        }

        case INSTRUCTION_SET::INVALID: {
            interpreter_error(line_number, "Invalid action " + instruction_vector[0],
                              instruction_vector);
        }
    }
}

