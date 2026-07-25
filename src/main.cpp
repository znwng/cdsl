#include "../include/file.hpp"
#include "../include/validation.hpp"

#include <cstdio>
#include <iostream>

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

