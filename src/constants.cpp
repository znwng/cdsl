#include "../include/constants.hpp"

#include <cstdio>
#include <iostream>
#include <unordered_map>

namespace {
std::unordered_map<std::string, float> constants;
}

void set_constant(const std::string& constant_key, float constant_value) {
    constants[constant_key] = constant_value;
}

bool has_constant(const std::string& name) { return constants.contains(name); }

float get_constant(const std::string& name) { return constants.at(name); }

void display_constants() {
    for (const auto& [key, value] : constants) {
        std::cout << key << " : " << value << '\n';
    }
}
