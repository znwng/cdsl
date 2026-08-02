#include <cctype>
#include <cstdio>
#include <expected>
#include <stdexcept>
#include <string>

std::expected<float, std::string> is_valid_float_value(const std::string& string_value) {
    try {
        size_t pos = 0;
        float value = std::stof(string_value, &pos);

        if (pos != string_value.size()) {
            return std::unexpected("Invalid value: " + string_value);
        }

        return value;
    } catch (const std::invalid_argument&) {
        return std::unexpected("Invalid value: " + string_value);
    } catch (const std::out_of_range&) {
        return std::unexpected("Value out of range");
    }
}

std::expected<int, std::string> is_valid_int_value(const std::string& string_value) {
    try {
        size_t pos = 0;
        int value = std::stoi(string_value, &pos);

        if (pos != string_value.size()) {
            return std::unexpected("Invalid value: " + string_value);
        }

        return value;
    } catch (const std::invalid_argument&) {
        return std::unexpected("Invalid value: " + string_value);
    } catch (const std::out_of_range&) {
        return std::unexpected("Value out of range");
    }
}

bool is_valid_variable_name(const std::string& name) {
    if (name.empty()) {
        return false;
    }

    // First character must be a letter or underscore
    if (!(std::isalpha(static_cast<unsigned char>(name[0])) || name[0] == '_')) {
        return false;
    }

    // Remaining characters must be letters, digits, or underscores
    for (size_t i = 1; i < name.size(); i++) {
        if (!(std::isalnum(static_cast<unsigned char>(name[i])) || name[i] == '_')) {
            return false;
        }
    }

    return true;
}
