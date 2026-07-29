#include "../include/file.hpp"
#include "../include/validation.hpp"

#include <CLI/CLI.hpp>
#include <cstdio>
#include <iostream>

int main(int argc, char* argv[]) {
    CLI::App app {"cdsl"};
    app.footer("Example:\n  cdsl program.cdsl");

    // Arguments
    std::string file_path;
    bool check = false;

    app.add_option("file", file_path, "Input file")->required();
    app.add_flag("--check", check, "Validate without executing");

    CLI11_PARSE(app, argc, argv);

    if (!is_valid_instruction_file(file_path)) {
        std::cerr << "Invalid file passed. `.cdsl` file required\n";
        return 1;
    }

    if (check) {
        // validate_without_executing(filename);
        validate_without_executing(file_path);
    } else {
        process_instructions_file(file_path);
    }

    return 0;
}

