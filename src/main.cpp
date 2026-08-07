#include <CLI/CLI.hpp>
#include <iostream>
#include <string>

#include "../include/file.hpp"
#include "../include/runtime.hpp"
#include "../include/validation.hpp"

int main(int argc, char* argv[]) {
    CLI::App app{"cdsl"};
    app.footer("Example:\n  cdsl program.cdsl");

    std::string file_path;
    bool check_flag = false;

    app.add_option("file", file_path, "Input .cdsl file");
    app.add_flag("--check", check_flag, "Validate without executing");

    CLI11_PARSE(app, argc, argv);

    if (file_path.empty()) {
        run_interactive_mode();
        return 0;
    }

    if (!is_valid_instruction_file(file_path)) {
        std::cerr << "Invalid file passed. `.cdsl` file required\n";
        return 1;
    }

    process_instructions_file(file_path, check_flag);

    return 0;
}
