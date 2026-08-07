#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../include/runtime.hpp"
#include "../include/types.hpp"

bool is_valid_instruction_file(const std::string& file_path) {
    std::filesystem::path path(file_path);
    return path.extension() == ".cdsl";
}

void process_instructions_file(const std::string& file_path, bool check_flag) {
    std::ifstream instructions_file(file_path);
    if (!instructions_file) {
        std::cerr << "Failed to open file: " << file_path << '\n';
        std::exit(EXIT_FAILURE);
    }

    std::string line;
    int line_number = 0;

    while (std::getline(instructions_file, line)) {
        line_number++;
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        Instruction tokens;
        std::string token;

        while (iss >> token) {
            if (token.starts_with("//")) {
                break;
            }
            tokens.push_back(token);
        }

        if (!tokens.empty()) {
            process_instruction(tokens, line_number, check_flag);
        }
    }
}
