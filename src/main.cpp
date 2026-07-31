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
    bool check_flag = false;

    app.add_option("file", file_path, "Input file")->required();
    app.add_flag("--check", check_flag, "Validate without executing");

    CLI11_PARSE(app, argc, argv);

    if (!is_valid_instruction_file(file_path)) {
        std::cerr << "Invalid file passed. `.cdsl` file required\n";
        return 1;
    }

    process_instructions_file(file_path, check_flag);

    return 0;
}

