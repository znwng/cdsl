#include "../include/variables.hpp"

#include <cstdio>
#include <iostream>
#include <unordered_map>

namespace {
std::unordered_map<std::string, float> variables;
}

void set_variable(const std::string& variable_key, float variable_value) { variables[variable_key] = variable_value; }

bool has_variable(const std::string& name) { return variables.contains(name); }

float get_variable(const std::string& name) { return variables.at(name); }

void clear_variables() { variables.clear(); }

void display_variables() {
    for (const auto& [key, value] : variables) {
        std::cout << key << " : " << value << '\n';
    }
}
