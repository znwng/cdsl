#include "../include/runtime.hpp"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <replxx.hxx>
#include <string>
#include <thread>
#include <unordered_map>

#include "../include/diagnostics.hpp"
#include "../include/types.hpp"
#include "../include/validation.hpp"
#include "../include/variables.hpp"

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

float evaluate_expression(const std::string& expression) {
    size_t pos = 0;

    auto skip_whitespace = [&]() {
        while (pos < expression.size() && std::isspace(static_cast<unsigned char>(expression[pos]))) {
            ++pos;
        }
    };

    std::function<float()> parse_expression;
    std::function<float()> parse_term;
    std::function<float()> parse_factor;
    std::function<float()> parse_primary;

    parse_primary = [&]() -> float {
        skip_whitespace();

        if (pos >= expression.size()) {
            throw std::runtime_error("Unexpected end of expression");
        }

        // Parenthesized expression
        if (expression[pos] == '(') {
            ++pos;

            float value = parse_expression();

            skip_whitespace();

            if (pos >= expression.size() || expression[pos] != ')') {
                throw std::runtime_error("Expected ')'");
            }

            ++pos;

            return value;
        }

        // variable: $variable_name
        if (expression[pos] == '$') {
            ++pos;

            size_t start = pos;

            if (pos >= expression.size() ||
                (!std::isalpha(static_cast<unsigned char>(expression[pos])) && expression[pos] != '_')) {
                throw std::runtime_error("Expected variable name after '$'");
            }

            ++pos;

            while (pos < expression.size() &&
                   (std::isalnum(static_cast<unsigned char>(expression[pos])) || expression[pos] == '_')) {
                ++pos;
            }

            std::string name = expression.substr(start, pos - start);

            if (!has_variable(name)) {
                throw std::runtime_error("Undefined variable: $" + name);
            }

            return get_variable(name);
        }

        // Number
        if (std::isdigit(static_cast<unsigned char>(expression[pos])) || expression[pos] == '.') {
            size_t start = pos;

            while (pos < expression.size() &&
                   (std::isdigit(static_cast<unsigned char>(expression[pos])) || expression[pos] == '.')) {
                ++pos;
            }

            return std::stof(expression.substr(start, pos - start));
        }

        throw std::runtime_error("Unexpected character: " + std::string(1, expression[pos]));
    };

    // Unary + and -
    parse_factor = [&]() -> float {
        skip_whitespace();

        if (pos < expression.size() && expression[pos] == '-') {
            ++pos;
            return -parse_factor();
        }

        if (pos < expression.size() && expression[pos] == '+') {
            ++pos;
            return parse_factor();
        }

        return parse_primary();
    };

    // * and /
    parse_term = [&]() -> float {
        float value = parse_factor();

        while (true) {
            skip_whitespace();

            if (pos >= expression.size()) {
                break;
            }

            char op = expression[pos];

            if (op != '*' && op != '/') {
                break;
            }

            ++pos;

            float rhs = parse_factor();

            if (op == '*') {
                value *= rhs;
            } else {
                if (rhs == 0.0f) {
                    throw std::runtime_error("Division by zero");
                }

                value /= rhs;
            }
        }

        return value;
    };

    // + and -
    parse_expression = [&]() -> float {
        float value = parse_term();

        while (true) {
            skip_whitespace();

            if (pos >= expression.size()) {
                break;
            }

            char op = expression[pos];

            if (op != '+' && op != '-') {
                break;
            }

            ++pos;

            float rhs = parse_term();

            if (op == '+') {
                value += rhs;
            } else {
                value -= rhs;
            }
        }

        return value;
    };

    float result = parse_expression();

    skip_whitespace();

    if (pos != expression.size()) {
        throw std::runtime_error("Unexpected character: " + std::string(1, expression[pos]));
    }

    return result;
}

}  // namespace

void process_instruction(const Instruction& instruction, int line_number, bool check_flag) {
    std::string action = instruction[0];

    static const std::unordered_map<std::string, INSTRUCTION_SET> opcode_table{
        {"set", INSTRUCTION_SET::SET},
        {"print", INSTRUCTION_SET::PRINT},
        {"move", INSTRUCTION_SET::MOVE},
        {"wait", INSTRUCTION_SET::WAIT},
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
                    interpreter_error_continue(line_number, "Invalid variable name: " + instruction[1], instruction);
                    break;
                } else {
                    interpreter_error(line_number, "Invalid variable name: " + instruction[1], instruction);
                }
            }

            std::string variable_key = instruction[1];
            float variable_value;

            // Expression
            if (instruction[2].starts_with("#[")) {
                if (instruction[2].size() < 3 || instruction[2].back() != ']') {
                    if (check_flag) {
                        interpreter_error_continue(line_number, "Invalid expression", instruction);
                        break;
                    } else {
                        interpreter_error(line_number, "Invalid expression", instruction);
                    }
                }

                std::string expression = instruction[2].substr(2, instruction[2].size() - 3);

                try {
                    variable_value = evaluate_expression(expression);
                } catch (const std::exception& e) {
                    if (check_flag) {
                        interpreter_error_continue(line_number, e.what(), instruction);
                        break;
                    } else {
                        interpreter_error(line_number, e.what(), instruction);
                    }
                }
            }

            // Regular float
            else {
                auto successful_conversion = is_valid_float_value(instruction[2]);

                if (!successful_conversion) {
                    if (check_flag) {
                        interpreter_error_continue(line_number, successful_conversion.error(), instruction);
                        break;
                    } else {
                        interpreter_error(line_number, successful_conversion.error(), instruction);
                    }
                }

                variable_value = *successful_conversion;
            }

            set_variable(variable_key, variable_value);

            if (check_flag) {
                std::cout << "variable " << variable_key << " set to " << variable_value << '\n';
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

            const std::string& argument = instruction[1];

            // Expression
            if (argument.starts_with("#[")) {
                if (argument.size() < 3 || argument.back() != ']') {
                    if (check_flag) {
                        interpreter_error_continue(line_number, "Invalid expression", instruction);
                        break;
                    } else {
                        interpreter_error(line_number, "Invalid expression", instruction);
                    }
                }

                std::string expression = argument.substr(2, argument.size() - 3);

                try {
                    float result = evaluate_expression(expression);
                    std::cout << result << '\n';
                } catch (const std::exception& e) {
                    if (check_flag) {
                        interpreter_error_continue(line_number, e.what(), instruction);
                        break;
                    } else {
                        interpreter_error(line_number, e.what(), instruction);
                    }
                }
            }

            // variable
            else {
                std::string variable_key = argument;

                if (!variable_key.empty() && variable_key[0] == '$') {
                    variable_key.erase(0, 1);
                }

                if (has_variable(variable_key)) {
                    std::cout << get_variable(variable_key) << '\n';
                } else {
                    if (check_flag) {
                        interpreter_error_continue(line_number, "No variable with name " + variable_key, instruction);
                        break;
                    } else {
                        interpreter_error(line_number, "No variable with name " + variable_key, instruction);
                    }
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
            const std::string& value = instruction[2];

            // Expression
            if (value.starts_with("#[")) {
                if (value.size() < 3 || value.back() != ']') {
                    if (check_flag) {
                        interpreter_error_continue(line_number, "Invalid expression", instruction);
                        break;
                    } else {
                        interpreter_error(line_number, "Invalid expression", instruction);
                    }
                }

                std::string expression = value.substr(2, value.size() - 3);

                try {
                    float result = evaluate_expression(expression);

                    if (!check_flag) {
                        move_function(component_label, result);
                    }
                } catch (const std::exception& e) {
                    if (check_flag) {
                        interpreter_error_continue(line_number, e.what(), instruction);
                        break;
                    } else {
                        interpreter_error(line_number, e.what(), instruction);
                    }
                }

                break;
            }

            // variable
            if (!value.empty() && value[0] == '$') {
                std::string variable_name = value.substr(1);

                if (!has_variable(variable_name)) {
                    if (check_flag) {
                        interpreter_error_continue(line_number, "Unknown variable: " + variable_name, instruction);
                        break;
                    } else {
                        interpreter_error(line_number, "Unknown variable: " + variable_name, instruction);
                    }
                }

                if (!check_flag) {
                    move_function(component_label, get_variable(variable_name));
                }

                break;
            }

            // Literal float
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

            const std::string& value = instruction[1];
            int delay;

            // Expression
            if (value.starts_with("#[")) {
                if (value.size() < 3 || value.back() != ']') {
                    if (check_flag) {
                        interpreter_error_continue(line_number, "Invalid expression", instruction);
                        break;
                    } else {
                        interpreter_error(line_number, "Invalid expression", instruction);
                    }
                }

                std::string expression = value.substr(2, value.size() - 3);

                try {
                    float result = evaluate_expression(expression);

                    delay = static_cast<int>(result);
                } catch (const std::exception& e) {
                    if (check_flag) {
                        interpreter_error_continue(line_number, e.what(), instruction);
                        break;
                    } else {
                        interpreter_error(line_number, e.what(), instruction);
                    }
                }
            }

            // variable
            else if (value.starts_with('$')) {
                std::string variable_name = value.substr(1);

                if (!has_variable(variable_name)) {
                    if (check_flag) {
                        interpreter_error_continue(line_number, "Unknown variable: " + variable_name, instruction);
                        break;
                    } else {
                        interpreter_error(line_number, "Unknown variable: " + variable_name, instruction);
                    }
                }

                delay = static_cast<int>(get_variable(variable_name));
            }

            // Literal integer
            else {
                auto successful_conversion = is_valid_int_value(value);

                if (!successful_conversion) {
                    if (check_flag) {
                        interpreter_error_continue(line_number, successful_conversion.error(), instruction);
                        break;
                    } else {
                        interpreter_error(line_number, successful_conversion.error(), instruction);
                    }
                }

                delay = *successful_conversion;
            }

            // Common validation
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
        {"set", INSTRUCTION_SET::SET},
        {"print", INSTRUCTION_SET::PRINT},
        {"move", INSTRUCTION_SET::MOVE},
        {"wait", INSTRUCTION_SET::WAIT},
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
            if (instruction.size() != 3) {
                std::cerr << "Invalid number of instructions. " << "Example: `SET variable_name value`" << '\n';
                break;
            }

            if (!is_valid_variable_name(instruction[1])) {
                std::cerr << "Invalid variable name: " << instruction[1] << '\n';
                break;
            }

            std::string variable_key = instruction[1];
            float variable_value;

            // Expression
            if (instruction[2].starts_with("#[")) {
                const std::string& value = instruction[2];

                if (value.size() < 3 || value.back() != ']') {
                    std::cerr << "Invalid expression\n";
                    break;
                }

                std::string expression = value.substr(2, value.size() - 3);

                try {
                    variable_value = evaluate_expression(expression);
                } catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                    break;
                }
            }

            // variable
            else if (instruction[2].starts_with('$')) {
                std::string variable_name = instruction[2].substr(1);

                if (!has_variable(variable_name)) {
                    std::cerr << "Unknown variable: " << variable_name << '\n';
                    break;
                }

                variable_value = get_variable(variable_name);
            }

            // Literal
            else {
                auto successful_conversion = is_valid_float_value(instruction[2]);

                if (!successful_conversion) {
                    std::cerr << successful_conversion.error() << '\n';
                    break;
                }

                variable_value = *successful_conversion;
            }

            set_variable(variable_key, variable_value);

            std::cout << "variable " << variable_key << " set to " << variable_value << '\n';

            break;
        }

        case INSTRUCTION_SET::PRINT: {
            if (instruction.size() != 2) {
                std::cerr << "Invalid number of arguments. " << "Example: `PRINT value`" << '\n';
                break;
            }

            const std::string& value = instruction[1];

            // Expression
            if (value.starts_with("#[")) {
                if (value.size() < 3 || value.back() != ']') {
                    std::cerr << "Invalid expression\n";
                    break;
                }

                std::string expression = value.substr(2, value.size() - 3);

                try {
                    std::cout << evaluate_expression(expression) << '\n';
                } catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                }

                break;
            }

            // variable
            std::string variable_key = value;

            if (variable_key.starts_with('$')) {
                variable_key.erase(0, 1);
            }

            if (!has_variable(variable_key)) {
                std::cerr << "No variable with name " << variable_key << '\n';
                break;
            }

            std::cout << get_variable(variable_key) << '\n';

            break;
        }

        case INSTRUCTION_SET::MOVE: {
            if (instruction.size() != 3) {
                std::cerr << "Invalid number of arguments. " << "Example: `MOVE COMPONENT_NAME VALUE`" << '\n';
                break;
            }

            std::string component_label = instruction[1];
            const std::string& value = instruction[2];

            // Expression
            if (value.starts_with("#[")) {
                if (value.size() < 3 || value.back() != ']') {
                    std::cerr << "Invalid expression\n";
                    break;
                }

                std::string expression = value.substr(2, value.size() - 3);

                try {
                    float result = evaluate_expression(expression);

                    move_function(component_label, result);
                } catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                }

                break;
            }

            // variable
            if (value.starts_with('$')) {
                std::string variable_name = value.substr(1);

                if (!has_variable(variable_name)) {
                    std::cerr << "Unknown variable: " << variable_name << '\n';
                    break;
                }

                move_function(component_label, get_variable(variable_name));

                break;
            }

            // Literal
            auto successful_conversion = is_valid_float_value(value);

            if (!successful_conversion) {
                std::cerr << successful_conversion.error() << '\n';
                break;
            }

            move_function(component_label, *successful_conversion);

            break;
        }

        case INSTRUCTION_SET::WAIT: {
            if (instruction.size() != 2) {
                std::cerr << "Invalid number of arguments. " << "Example: `WAIT DURATION_MS`" << '\n';
                break;
            }

            const std::string& value = instruction[1];
            int delay;

            // Expression
            if (value.starts_with("#[")) {
                if (value.size() < 3 || value.back() != ']') {
                    std::cerr << "Invalid expression\n";
                    break;
                }

                std::string expression = value.substr(2, value.size() - 3);

                try {
                    float result = evaluate_expression(expression);

                    delay = static_cast<int>(result);
                } catch (const std::exception& e) {
                    std::cerr << e.what() << '\n';
                    break;
                }
            }

            // variable
            else if (value.starts_with('$')) {
                std::string variable_name = value.substr(1);

                if (!has_variable(variable_name)) {
                    std::cerr << "Unknown variable: " << variable_name << '\n';
                    break;
                }

                delay = static_cast<int>(get_variable(variable_name));
            }

            // Literal
            else {
                auto successful_conversion = is_valid_int_value(value);

                if (!successful_conversion) {
                    std::cerr << successful_conversion.error() << '\n';
                    break;
                }

                delay = *successful_conversion;
            }

            if (delay < 0) {
                std::cerr << "Delay cannot be negative\n";
                break;
            }

            wait_function(delay);

            break;
        }

        case INSTRUCTION_SET::INVALID: {
            std::cerr << "Invalid action\n";
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

            if (start == std::string::npos) {
                break;
            }

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

        if (command.empty()) {
            continue;
        }

        rx.history_add(command);

        if (command == "exit") {
            break;
        }

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
