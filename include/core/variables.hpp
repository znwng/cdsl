#pragma once

#include <string>

void set_variable(const std::string& name, float value);

bool has_variable(const std::string& name);

float get_variable(const std::string& name);

void display_variables();

void clear_variables();
