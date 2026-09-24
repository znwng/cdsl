#include "core/runtime.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <replxx.hxx>
#include <sstream>
#include <string>
#include <unordered_map>

#include "core/color.hpp"
#include "core/config.hpp"
#include "core/diagnostics.hpp"
#include "core/parser.hpp"
#include "instructions/instruction.hpp"

enum class INSTRUCTION_SET { SET, PRINT, MOVE, WAIT, INVALID };

INSTRUCTION_SET get_opcode(const std::string& action) {
    static const std::unordered_map<std::string, INSTRUCTION_SET> OPCODE_TABLE{
        {"set", INSTRUCTION_SET::SET},
        {"print", INSTRUCTION_SET::PRINT},
        {"move", INSTRUCTION_SET::MOVE},
        {"wait", INSTRUCTION_SET::WAIT},
    };

    auto itr = OPCODE_TABLE.find(action);

    if (itr == OPCODE_TABLE.end()) {
        return INSTRUCTION_SET::INVALID;
    }

    return itr->second;
}

void process_instruction(const toml::table& config, const Instruction& instruction) {
    if (instruction.empty()) {
        return;
    }

    switch (get_opcode(instruction[0])) {
        case INSTRUCTION_SET::SET:
            process_set(instruction);
            break;

        case INSTRUCTION_SET::PRINT:
            process_print(instruction);
            break;

        case INSTRUCTION_SET::MOVE:
            process_move(config, instruction);
            break;

        case INSTRUCTION_SET::WAIT:
            process_wait(instruction);
            break;

        case INSTRUCTION_SET::INVALID:
            diagnostics::error("Invalid action: " + instruction[0], instruction);
            break;
    }
}

void run_interactive_mode() {
    // Load configuration once.
    auto config = config::load_config();

    if (!config) {
        std::cerr << config.error() << '\n';
        return;
    }

    const char* home = std::getenv("HOME");

    if (home == nullptr) {
        std::cerr << "Unable to determine home directory\n";
        return;
    }

    const std::filesystem::path HISTORY_FILE = std::filesystem::path(home) / ".cdsl_history";

    std::cout << "Starting interactive mode\n"
              << "Enter Ctrl+C, Ctrl+D, or type 'exit' to exit.\n"
              << "Type 'clear' to clear the screen.\n\n";

    replxx::Replxx replx;

    replx.set_highlighter_callback([](std::string const& input, replxx::Replxx::colors_t& colors) {
        using replxx::Replxx;

        std::ranges::fill(colors.begin(), colors.end(), Replxx::Color::WHITE);

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
                std::fill(
                    colors.begin() + static_cast<std::ptrdiff_t>(start),
                    colors.begin() + static_cast<std::ptrdiff_t>(start) + static_cast<std::ptrdiff_t>(token.size()),
                    Replxx::Color::GREEN);
            }

            pos = start + token.size();
        }
    });

    replx.history_load(HISTORY_FILE.string());

    while (true) {
        char const* input = replx.input(std::string(Color::YELLOW) + "cdsl> " + Color::RESET);

        if (input == nullptr) {
            std::cout << '\n';
            break;
        }

        std::string command(input);

        if (command.empty()) {
            continue;
        }

        replx.history_add(command);

        if (command == "exit") {
            break;
        }

        if (command == "clear") {
            replx.clear_screen();
            continue;
        }

        Instruction instruction = tokenize(command);

        if (!instruction.empty()) {
            process_instruction(*config, instruction);
        }
    }

    replx.history_save(HISTORY_FILE.string());
}
