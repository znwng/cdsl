#include "core/expression.hpp"

#include <cctype>
#include <charconv>
#include <string>

#include "core/variables.hpp"

namespace {

void skip_whitespace(const std::string& expression, size_t& pos) {
    while (pos < expression.size() && std::isspace(static_cast<unsigned char>(expression[pos])) != 0) {
        ++pos;
    }
}

std::expected<float, std::string> parse_expression(const std::string& expression, size_t& pos);

std::expected<float, std::string> parse_term(const std::string& expression, size_t& pos);

std::expected<float, std::string> parse_factor(const std::string& expression, size_t& pos);

std::expected<float, std::string> parse_primary(const std::string& expression, size_t& pos);

std::expected<float, std::string> parse_parenthesized(const std::string& expression, size_t& pos) {
    ++pos;  // Skip '('

    auto value = parse_expression(expression, pos);

    if (!value) {
        return std::unexpected(value.error());
    }

    skip_whitespace(expression, pos);

    if (pos >= expression.size() || expression[pos] != ')') {
        return std::unexpected("Expected ')'");
    }

    ++pos;  // Skip ')'

    return *value;
}

std::expected<float, std::string> parse_variable(const std::string& expression, size_t& pos) {
    ++pos;  // Skip '$'

    const size_t START = pos;

    if (pos >= expression.size() ||
        (std::isalpha(static_cast<unsigned char>(expression[pos])) == 0 && expression[pos] != '_')) {
        return std::unexpected("Expected variable name after '$'");
    }

    ++pos;

    while (pos < expression.size() &&
           (std::isalnum(static_cast<unsigned char>(expression[pos])) != 0 || expression[pos] == '_')) {
        ++pos;
    }

    const std::string NAME = expression.substr(START, pos - START);

    if (!has_variable(NAME)) {
        return std::unexpected("Undefined variable: $" + NAME);
    }

    return get_variable(NAME);
}

std::expected<float, std::string> parse_number(const std::string& expression, size_t& pos) {
    const size_t START = pos;

    bool has_digit = false;
    bool has_dot = false;

    while (pos < expression.size()) {
        const char CHARACTER = expression[pos];

        if (std::isdigit(static_cast<unsigned char>(CHARACTER)) != 0) {
            has_digit = true;
            ++pos;
            continue;
        }

        if (CHARACTER == '.' && !has_dot) {
            has_dot = true;
            ++pos;
            continue;
        }

        break;
    }

    if (!has_digit) {
        return std::unexpected("Invalid number");
    }

    const std::string NUMBER = expression.substr(START, pos - START);

    float value{};

    const auto [PTR, ERROR] = std::from_chars(NUMBER.data(), NUMBER.data() + NUMBER.size(), value);

    if (ERROR != std::errc{} || PTR != NUMBER.data() + NUMBER.size()) {
        return std::unexpected("Invalid number: " + NUMBER);
    }

    return value;
}

// ============================================================
// Primary
//
// Handles:
//
//     numbers
//     variables
//     (expressions)
// ============================================================

std::expected<float, std::string> parse_primary(const std::string& expression, size_t& pos) {
    skip_whitespace(expression, pos);

    if (pos >= expression.size()) {
        return std::unexpected("Unexpected end of expression");
    }

    switch (expression[pos]) {
        case '(':
            return parse_parenthesized(expression, pos);

        case '$':
            return parse_variable(expression, pos);

        default:
            break;
    }

    if (std::isdigit(static_cast<unsigned char>(expression[pos])) != 0 || expression[pos] == '.') {
        return parse_number(expression, pos);
    }

    return std::unexpected("Unexpected character: " + std::string(1, expression[pos]));
}

// ============================================================
// Factor
//
// Handles:
//
//     unary +
//     unary -
//
// Example:
//
//     -5
//     +5
//     --5
// ============================================================

std::expected<float, std::string> parse_factor(const std::string& expression, size_t& pos) {
    skip_whitespace(expression, pos);

    if (pos < expression.size() && expression[pos] == '-') {
        ++pos;

        auto value = parse_factor(expression, pos);

        if (!value) {
            return std::unexpected(value.error());
        }

        return -*value;
    }

    if (pos < expression.size() && expression[pos] == '+') {
        ++pos;

        return parse_factor(expression, pos);
    }

    return parse_primary(expression, pos);
}

// ============================================================
// Term
//
// Handles:
//
//     multiplication
//     division
//
// Example:
//
//     2 * 3
//     10 / 2
//     2 * 3 / 4
// ============================================================

std::expected<float, std::string> parse_term(const std::string& expression, size_t& pos) {
    auto value = parse_factor(expression, pos);

    if (!value) {
        return std::unexpected(value.error());
    }

    while (true) {
        skip_whitespace(expression, pos);

        if (pos >= expression.size()) {
            break;
        }

        const char OPERATION = expression[pos];

        if (OPERATION != '*' && OPERATION != '/') {
            break;
        }

        ++pos;

        auto rhs = parse_factor(expression, pos);

        if (!rhs) {
            return std::unexpected(rhs.error());
        }

        if (OPERATION == '*') {
            *value *= *rhs;
        } else {
            if (*rhs == 0.0F) {
                return std::unexpected("Division by zero");
            }

            *value /= *rhs;
        }
    }

    return *value;
}

// ============================================================
// Expression
//
// Handles:
//
//     addition
//     subtraction
//
// Example:
//
//     2 + 3
//     10 - 4
//     2 + 3 - 1
// ============================================================

std::expected<float, std::string> parse_expression(const std::string& expression, size_t& pos) {
    auto value = parse_term(expression, pos);

    if (!value) {
        return std::unexpected(value.error());
    }

    while (true) {
        skip_whitespace(expression, pos);

        if (pos >= expression.size()) {
            break;
        }

        const char OPERATION = expression[pos];

        if (OPERATION != '+' && OPERATION != '-') {
            break;
        }

        ++pos;

        auto rhs = parse_term(expression, pos);

        if (!rhs) {
            return std::unexpected(rhs.error());
        }

        if (OPERATION == '+') {
            *value += *rhs;
        } else {
            *value -= *rhs;
        }
    }

    return *value;
}

}  // namespace

// ============================================================
// Public API
// ============================================================

std::expected<float, std::string> evaluate_expression(const std::string& expression) {
    size_t pos = 0;

    auto result = parse_expression(expression, pos);

    if (!result) {
        return std::unexpected(result.error());
    }

    skip_whitespace(expression, pos);

    if (pos != expression.size()) {
        return std::unexpected("Unexpected character: " + std::string(1, expression[pos]));
    }

    return *result;
}
