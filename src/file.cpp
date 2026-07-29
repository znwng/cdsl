#include "../include/runtime.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void process_instructions_file(const std::string& file_path) {
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
        std::vector<std::string> tokens;
        std::string token;

        while (iss >> token) {
            // Stop reading tokens from the comment character onwards `//`
            if (token.starts_with("//")) {
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

void validate_without_executing(const std::string& file_path) {
    std::cout << "Validating " << file_path << std::endl;
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
        std::vector<std::string> tokens;
        std::string token;

        while (iss >> token) {
            // Stop reading tokens from the comment character onwards `//`
            if (token.starts_with("//")) {
                break;
            }
            tokens.push_back(token);
        }

        if (!tokens.empty()) {
            validate_instruction(tokens, line_number);
        }
    }
}

