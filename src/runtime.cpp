#include "../include/runtime.hpp"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <replxx.hxx>
#include <string>
#include <thread>
#include <unordered_map>

#include "../include/constants.hpp"
#include "../include/diagnostics.hpp"
#include "../include/types.hpp"
#include "../include/validation.hpp"

namespace {

enum class INSTRUCTION_SET { SET, PRINT, MOVE, WAIT, INVALID };

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

}  // namespace

void process_instruction(const Instruction& instruction, int line_number, bool check_flag) {
    std::string action = instruction[0];

    static const std::unordered_map<std::string, INSTRUCTION_SET> opcode_table{
        {"SET", INSTRUCTION_SET::SET},
        {"PRINT", INSTRUCTION_SET::PRINT},
        {"MOVE", INSTRUCTION_SET::MOVE},
        {"WAIT", INSTRUCTION_SET::WAIT},
    };

    INSTRUCTION_SET opcode;

    auto it = opcode_table.find(action);

    // Get the opcode from INSTRUCTION_SET enum
    if (it == opcode_table.end()) {
        opcode = INSTRUCTION_SET::INVALID;
    } else {
        opcode = it->second;
    }

    switch (opcode) {
        case INSTRUCTION_SET::SET: {
            if (instruction.size() != 3) {
                if (check_flag) {
                    interpreter_error_continue(line_number, "Invalid number of arguments", instruction);
                    break;
                } else {
                    interpreter_error(line_number, "Invalid number of arguments", instruction);
                }
            }

            if (!is_valid_variable_name(instruction[1])) {
                if (check_flag) {
                    interpreter_error_continue(line_number, "Invalid constant name: " + instruction[1], instruction);
                    break;
                } else {
                    interpreter_error(line_number, "Invalid constant name: " + instruction[1], instruction);
                }
            }
            std::string constant_key = instruction[1];
            auto successful_conversion = is_valid_float_value(instruction[2]);

            if (!successful_conversion) {
                if (check_flag) {
                    interpreter_error_continue(line_number, successful_conversion.error(), instruction);
                    break;
                } else {
                    interpreter_error(line_number, successful_conversion.error(), instruction);
                }
            }

            float constant_value = *successful_conversion;
            set_constant(constant_key, constant_value);
            if (check_flag) {
                std::cout << "Constant " << constant_key << " set to " << constant_value << std::endl;
            }
            break;
        }

        case INSTRUCTION_SET::PRINT: {
            if (instruction.size() != 2) {
                if (check_flag) {
                    interpreter_error_continue(line_number, "Invalid number of arguments", instruction);
                    break;
                } else {
                    interpreter_error(line_number, "Invalid number of arguments", instruction);
                }
            }

            std::string constant_key = instruction[1];

            if (!constant_key.empty() && constant_key[0] == '$') constant_key.erase(0, 1);

            if (has_constant(constant_key)) {
                std::cout << get_constant(constant_key) << '\n';
            } else {
                if (check_flag) {
                    interpreter_error_continue(line_number, "No constant with name " + constant_key, instruction);
                    break;
                } else {
                    interpreter_error(line_number, "No constant with name " + constant_key, instruction);
                }
            }

            break;
        }

        case INSTRUCTION_SET::MOVE: {
            if (instruction.size() != 3) {
                if (check_flag) {
                    interpreter_error_continue(line_number, "Invalid number of arguments", instruction);
                    break;
                } else {
                    interpreter_error(line_number, "Invalid number of arguments", instruction);
                }
            }
            std::string component_label = instruction[1];
            std::string value = instruction[2];
            if (value[0] == '$') {
                value.erase(0, 1);

                if (!has_constant(value)) {
                    if (check_flag) {
                        interpreter_error_continue(line_number, "Unknown constant: " + value, instruction);
                        break;
                    } else {
                        interpreter_error(line_number, "Unknown constant: " + value, instruction);
                    }
                }

                move_function(component_label, get_constant(value));
                break;
            }

            auto successful_conversion = is_valid_float_value(value);
            if (!successful_conversion) {
                if (check_flag) {
                    interpreter_error_continue(line_number, successful_conversion.error(), instruction);
                    break;
                } else {
                    interpreter_error(line_number, successful_conversion.error(), instruction);
                }
            }

            if (!check_flag) {
                move_function(component_label, *successful_conversion);
            }
            break;
        }

        case INSTRUCTION_SET::WAIT: {
            if (instruction.size() != 2) {
                if (check_flag) {
                    interpreter_error_continue(line_number, "Invalid number of arguments", instruction);
                    break;
                } else {
                    interpreter_error(line_number, "Invalid number of arguments", instruction);
                }
            }

            auto successful_conversion = is_valid_int_value(instruction[1]);

            if (!successful_conversion) {
                if (check_flag) {
                    interpreter_error_continue(line_number, successful_conversion.error(), instruction);
                    break;
                } else {
                    interpreter_error(line_number, successful_conversion.error(), instruction);
                }
            }

            int delay = *successful_conversion;
            if (delay < 0) {
                if (check_flag) {
                    interpreter_error_continue(line_number, "Delay cannot be negative " + std::to_string(delay),
                                               instruction);
                    break;
                } else {
                    interpreter_error(line_number, "Delay cannot be negative " + std::to_string(delay), instruction);
                }
            }

            if (!check_flag) {
                wait_function(delay);
            }
            break;
        }

        case INSTRUCTION_SET::INVALID: {
            if (check_flag) {
                interpreter_error_continue(line_number, "Invalid action " + instruction[0], instruction);
                break;

            } else {
                interpreter_error(line_number, "Invalid action " + instruction[0], instruction);
            }
        }
    }
}

void process_interactive_instruction(const Instruction& instruction) {
    std::string action = instruction[0];

    static const std::unordered_map<std::string, INSTRUCTION_SET> opcode_table{
        {"SET", INSTRUCTION_SET::SET},
        {"PRINT", INSTRUCTION_SET::PRINT},
        {"MOVE", INSTRUCTION_SET::MOVE},
        {"WAIT", INSTRUCTION_SET::WAIT},
    };

    INSTRUCTION_SET opcode;

    auto it = opcode_table.find(action);

    // Get the opcode from INSTRUCTION_SET enum
    if (it == opcode_table.end()) {
        opcode = INSTRUCTION_SET::INVALID;
    } else {
        opcode = it->second;
    }

    switch (opcode) {
        case INSTRUCTION_SET::SET: {
            if (instruction.size() != 3) {
                std::cerr << "Invalid number of instructions. Example: `SET constant_name constant_value`" << std::endl;
                break;
            }

            if (!is_valid_variable_name(instruction[1])) {
                std::cerr << "Invalid constant name: " << instruction[1] << std::endl;
                break;
            }
            std::string constant_key = instruction[1];
            auto successful_conversion = is_valid_float_value(instruction[2]);

            if (!successful_conversion) {
                std::cerr << successful_conversion.error() << std::endl;
                break;
            }

            float constant_value = *successful_conversion;
            set_constant(constant_key, constant_value);
            std::cout << "Constant " << constant_key << " set to " << constant_value << std::endl;
            break;
        }

        case INSTRUCTION_SET::PRINT: {
            if (instruction.size() != 2) {
                std::cerr << "Invalid number of arguments. Example: `PRINT constant_name`" << std::endl;
                break;
            }

            std::string constant_key = instruction[1];

            if (!constant_key.empty() && constant_key[0] == '$') constant_key.erase(0, 1);

            if (has_constant(constant_key)) {
                std::cout << get_constant(constant_key) << '\n';
            } else {
                std::cerr << "No constant with name " << constant_key << std::endl;
                break;
            }

            break;
        }

        case INSTRUCTION_SET::MOVE: {
            if (instruction.size() != 3) {
                std::cerr << "Invalid number of arguments. Example: `MOVE COMPONENT_NAME VALUE`" << std::endl;
                break;
            }
            std::string component_label = instruction[1];
            std::string value = instruction[2];
            if (value[0] == '$') {
                value.erase(0, 1);

                if (!has_constant(value)) {
                    std::cerr << "Unknown constant: " << value << std::endl;
                    break;
                }

                move_function(component_label, get_constant(value));
                break;
            }

            auto successful_conversion = is_valid_float_value(value);
            if (!successful_conversion) {
                std::cerr << successful_conversion.error() << std::endl;
                break;
            }

            move_function(component_label, *successful_conversion);
            break;
        }

        case INSTRUCTION_SET::WAIT: {
            if (instruction.size() != 2) {
                std::cerr << "Invalid number of arguments. Example: `WAIT DURATION_MS`" << std::endl;
                break;
            }

            auto successful_conversion = is_valid_int_value(instruction[1]);

            if (!successful_conversion) {
                std::cerr << successful_conversion.error() << std::endl;
                break;
            }

            int delay = *successful_conversion;
            if (delay < 0) {
                std::cerr << "Delay cannot be negative" << std::endl;
                break;
            }

            wait_function(delay);
            break;
        }

        case INSTRUCTION_SET::INVALID: {
            std::cerr << "Invalid action" << std::endl;
            break;
        }
    }
}

void run_interactive_mode() {
    std::cout << "Starting interactive mode\n"
              << "Enter Ctrl+C, Ctrl+D, or type 'exit' to exit.\n"
              << "Type 'clear' to clear the screen.\n\n";

    replxx::Replxx rx;

    // Highlight only numbers in green.
    rx.set_highlighter_callback([](std::string const& input, replxx::Replxx::colors_t& colors) {
        using replxx::Replxx;

        std::fill(colors.begin(), colors.end(), Replxx::Color::WHITE);

        std::istringstream iss(input);
        std::string token;
        std::size_t pos = 0;

        while (iss >> token) {
            std::size_t start = input.find(token, pos);
            if (start == std::string::npos) break;

            char* end = nullptr;
            std::strtof(token.c_str(), &end);

            if (end != token.c_str() && *end == '\0') {
                std::fill(colors.begin() + start, colors.begin() + start + token.size(), Replxx::Color::GREEN);
            }

            pos = start + token.size();
        }
    });

    rx.history_load(".cdsl_history");

    while (true) {
        char const* input = rx.input((std::string(Color::YELLOW) + "cdsl> " + Color::RESET).c_str());

        if (input == nullptr) {
            std::cout << '\n';
            break;
        }

        std::string command(input);

        if (command.empty()) continue;

        rx.history_add(command);

        if (command == "exit") break;

        if (command == "clear") {
            rx.clear_screen();
            continue;
        }

        std::istringstream iss(command);
        Instruction tokens;
        std::string token;

        while (iss >> token) {
            if (token.starts_with("//")) {
                break;
            }

            tokens.push_back(token);
        }

        if (!tokens.empty()) {
            process_interactive_instruction(tokens);
        }
    }

    rx.history_save(".cdsl_history");
}
