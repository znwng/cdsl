#include "../include/runtime.hpp"

#include <iostream>
#include <replxx.hxx>
#include <sstream>
#include <string>
#include <unordered_map>

#include "../include/diagnostics.hpp"
#include "../include/instructions/instruction.hpp"

namespace {

enum class INSTRUCTION_SET { SET, PRINT, MOVE, WAIT, INVALID };

INSTRUCTION_SET get_opcode(const std::string& action) {
    static const std::unordered_map<std::string, INSTRUCTION_SET> opcode_table{
        {"set", INSTRUCTION_SET::SET},
        {"print", INSTRUCTION_SET::PRINT},
        {"move", INSTRUCTION_SET::MOVE},
        {"wait", INSTRUCTION_SET::WAIT},
    };

    auto it = opcode_table.find(action);

    if (it == opcode_table.end()) {
        return INSTRUCTION_SET::INVALID;
    }

    return it->second;
}

}  // namespace

void process_instruction(const Instruction& instruction, int line_number, bool check_flag) {
    if (instruction.empty()) {
        return;
    }

    switch (get_opcode(instruction[0])) {
        case INSTRUCTION_SET::SET:
            instructions::process_set(instruction, line_number, check_flag);
            break;

        case INSTRUCTION_SET::PRINT:
            instructions::process_print(instruction, line_number, check_flag);
            break;

        case INSTRUCTION_SET::MOVE:
            instructions::process_move(instruction, line_number, check_flag);
            break;

        case INSTRUCTION_SET::WAIT:
            instructions::process_wait(instruction, line_number, check_flag);
            break;

        case INSTRUCTION_SET::INVALID:
            if (check_flag) {
                interpreter_error_continue(line_number, "Invalid action " + instruction[0], instruction);
            } else {
                interpreter_error(line_number, "Invalid action " + instruction[0], instruction);
            }
            break;
    }
}

void process_interactive_instruction(const Instruction& instruction) {
    if (instruction.empty()) {
        return;
    }

    switch (get_opcode(instruction[0])) {
        case INSTRUCTION_SET::SET:
            instructions::process_interactive_set(instruction);
            break;

        case INSTRUCTION_SET::PRINT:
            instructions::process_interactive_print(instruction);
            break;

        case INSTRUCTION_SET::MOVE:
            instructions::process_interactive_move(instruction);
            break;

        case INSTRUCTION_SET::WAIT:
            instructions::process_interactive_wait(instruction);
            break;

        case INSTRUCTION_SET::INVALID:
            std::cerr << "Invalid action\n";
            break;
    }
}

void run_interactive_mode() {
    std::cout << "Starting interactive mode\n"
              << "Enter Ctrl+C, Ctrl+D, or type 'exit' to exit.\n"
              << "Type 'clear' to clear the screen.\n\n";

    replxx::Replxx rx;

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
