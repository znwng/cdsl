#include <CLI/CLI.hpp>
#include <iostream>
#include <string>

#include "../include/file.hpp"
#include "../include/runtime.hpp"
#include "../include/validation.hpp"

int main(int argc, char* argv[]) {
    CLI::App app{"CDSL - Command Description and Scripting Language"};

    app.description(
        "CDSL is a command-line interpreter for executing robot control\n"
        "instructions interactively or from a script file.");

    std::string file_path;
    std::string instruction;
    bool check_flag = false;

    app.add_option("file", file_path, "Input .cdsl file");
    app.add_flag("--check,-c", check_flag, "Validate the specified CDSL file without executing it.");
    app.add_option("send", instruction, "Instruction");

    CLI11_PARSE(app, argc, argv);

    if (!instruction.empty()) {
        process_instruction(tokenize(instruction), 0, check_flag);
        return 0;
    }

    if (file_path.empty()) {
        if (check_flag) {
            std::cerr << "Error: --check requires a .cdsl file.\n";
            return 1;
        }

        run_interactive_mode();
        return 0;
    }

    if (!is_valid_instruction_file(file_path)) {
        std::cerr << "Invalid file passed. `.cdsl` file required.\n";
        return 1;
    }

    process_instructions_file(file_path, check_flag);

    return 0;
}
