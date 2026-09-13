#include <cstdio>
#include <sstream>
#include <string>

#include "types.hpp"

Instruction tokenize(const std::string& line) {
    std::istringstream iss(line);
    Instruction tokens;
    std::string token;

    while (iss >> token) {
        if (token.starts_with("//")) {
            break;
        }

        tokens.push_back(token);
    }

    return tokens;
}
