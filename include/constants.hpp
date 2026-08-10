#pragma once

#include <string>

void set_constant(const std::string& name, float value);

bool has_constant(const std::string& name);

float get_constant(const std::string& name);

void display_constants();

void clear_constants();
