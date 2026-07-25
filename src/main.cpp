#include <chrono>
#include <cstdio>
#include <expected>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace Color {

constexpr const char* RESET = "\033[0m";
constexpr const char* RED = "\033[31m";
constexpr const char* GREEN = "\033[32m";
constexpr const char* YELLOW = "\033[33m";
constexpr const char* BLUE = "\033[34m";
constexpr const char* CYAN = "\033[36m";

} // namespace Color

enum class INSTRUCTION_SET { SET, MOVE, WAIT, INVALID };

std::unordered_map<std::string, float> constants;
void display_constants_map() {
    for (const auto& [key, value] : constants) {
        std::cout << key << " : " << value << '\n';
    }
}

void set_constant_function(const std::string& constant_key, float constant_value) {
    constants[constant_key] = constant_value;
    std::printf("Constant %s set as %f\n\n", constant_key.c_str(), constant_value);
}

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

std::expected<float, std::string> is_valid_float_value(const std::string& string_value) {
    try {
        size_t pos = 0;
        float value = std::stof(string_value, &pos);

        if (pos != string_value.size()) {
            return std::unexpected("Invalid value: " + string_value);
        }

        return value;
    } catch (const std::invalid_argument&) {
        return std::unexpected("Invalid value: " + string_value);
    } catch (const std::out_of_range&) {
        return std::unexpected("Value out of range");
    }
}

std::expected<int, std::string> is_valid_int_value(const std::string& string_value) {
    try {
        size_t pos = 0;
        int value = std::stoi(string_value, &pos);

        if (pos != string_value.size()) {
            return std::unexpected("Invalid value: " + string_value);
        }

        return value;
    } catch (const std::invalid_argument&) {
        return std::unexpected("Invalid value: " + string_value);
    } catch (const std::out_of_range&) {
        return std::unexpected("Value out of range");
    }
}

bool is_valid_variable_name(const std::string& name) {
    if (name.empty()) {
        return false;
    }

    // First character must be a letter or underscore
    if (!(std::isalpha(static_cast<unsigned char>(name[0])) || name[0] == '_')) {
        return false;
    }

    // Remaining characters must be letters, digits, or underscores
    for (size_t i = 1; i < name.size(); i++) {
        if (!(std::isalnum(static_cast<unsigned char>(name[i])) || name[i] == '_')) {
            return false;
        }
    }

    return true;
}

void display_instruction(const std::vector<std::string>& instruction_vector) {
    for (const std::string& token : instruction_vector) {
        std::cout << token << ' ';
    }
    std::cout << std::endl;
}

[[noreturn]]
void interpreter_error(int line_number, const std::string& message,
                       const std::vector<std::string>& instruction_vector) {
    std::cerr << Color::RED << "Line " << line_number << '\n' << message << Color::RESET << '\n';
    display_instruction(instruction_vector);
    std::exit(EXIT_FAILURE);
}

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
            set_constant_function(constant_key, constant_value);
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

                auto it = constants.find(value);
                if (it == constants.end()) {
                    interpreter_error(line_number, "Unknown constant: " + value,
                                      instruction_vector);
                }

                move_function(component_label, it->second);
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
            std::printf("\n");
        }
    }
}

void process_instructions_file(const std::string& file_path) {
    std::ifstream instructions_file(file_path);
    std::string line;
    int line_number = 0;

    while (std::getline(instructions_file, line)) {
        line_number++;
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (iss >> token) {
            if (token.starts_with(
                    "//")) { // Stop reading tokens from the comment character onwards `//`
                break;
            }
            tokens.push_back(token);
        }

        if (!tokens.empty()) {
            execute_instruction(tokens, line_number);
        }
    }
}

bool is_valid_instruction_file(const std::string& file_path) {
    std::filesystem::path path(file_path);
    return path.extension() == ".cdsl";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "\nUsage:\n\tcdsl <instruction_set_file>\n\n";
        return 1;
    }

    if (!is_valid_instruction_file(argv[1])) {
        std::cerr << "Invalid file passed. `.cdsl` file required\n";
        return 1;
    }

    process_instructions_file(argv[1]);

    return 0;
}

