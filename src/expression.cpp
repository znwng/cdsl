#include "expression.hpp"

#include <cctype>
#include <stdexcept>
#include <string>

#include "variables.hpp"

void skip_whitespace(const std::string& expression, size_t& pos) {
    while (pos < expression.size() && (std::isspace(static_cast<unsigned char>(expression[pos])) != 0)) {
        ++pos;
    }
}

float parse_expression(const std::string& expression, size_t& pos);
float parse_term(const std::string& expression, size_t& pos);
float parse_factor(const std::string& expression, size_t& pos);
float parse_primary(const std::string& expression, size_t& pos);

float parse_primary(const std::string& expression, size_t& pos) {
    skip_whitespace(expression, pos);

    if (pos >= expression.size()) {
        throw std::runtime_error("Unexpected end of expression");
    }

    // Parenthesized expression
    if (expression[pos] == '(') {
        ++pos;

        float value = parse_expression(expression, pos);

        skip_whitespace(expression, pos);

        if (pos >= expression.size() || expression[pos] != ')') {
            throw std::runtime_error("Expected ')'");
        }

        ++pos;

        return value;
    }

    // Variable: $variable_name
    if (expression[pos] == '$') {
        ++pos;

        size_t start = pos;

        if (pos >= expression.size() ||
            ((std::isalpha(static_cast<unsigned char>(expression[pos])) == 0) && expression[pos] != '_')) {
            throw std::runtime_error("Expected variable name after '$'");
        }

        ++pos;

        while (pos < expression.size() &&
               ((std::isalnum(static_cast<unsigned char>(expression[pos])) != 0) || expression[pos] == '_')) {
            ++pos;
        }

        std::string name = expression.substr(start, pos - start);

        if (!has_variable(name)) {
            throw std::runtime_error("Undefined variable: $" + name);
        }

        return get_variable(name);
    }

    // Number
    if ((std::isdigit(static_cast<unsigned char>(expression[pos])) != 0) || expression[pos] == '.') {
        size_t start = pos;

        while (pos < expression.size() &&
               ((std::isdigit(static_cast<unsigned char>(expression[pos])) != 0) || expression[pos] == '.')) {
            ++pos;
        }

        return std::stof(expression.substr(start, pos - start));
    }

    throw std::runtime_error("Unexpected character: " + std::string(1, expression[pos]));
}

// Unary + and -
float parse_factor(const std::string& expression, size_t& pos) {
    skip_whitespace(expression, pos);

    if (pos < expression.size() && expression[pos] == '-') {
        ++pos;
        return -parse_factor(expression, pos);
    }

    if (pos < expression.size() && expression[pos] == '+') {
        ++pos;
        return parse_factor(expression, pos);
    }

    return parse_primary(expression, pos);
}

// * and /
float parse_term(const std::string& expression, size_t& pos) {
    float value = parse_factor(expression, pos);

    while (true) {
        skip_whitespace(expression, pos);

        if (pos >= expression.size()) {
            break;
        }

        char op = expression[pos];

        if (op != '*' && op != '/') {
            break;
        }

        ++pos;

        float rhs = parse_factor(expression, pos);

        if (op == '*') {
            value *= rhs;
        } else {
            if (rhs == 0.0F) {
                throw std::runtime_error("Division by zero");
            }

            value /= rhs;
        }
    }

    return value;
}

// + and -
float parse_expression(const std::string& expression, size_t& pos) {
    float value = parse_term(expression, pos);

    while (true) {
        skip_whitespace(expression, pos);

        if (pos >= expression.size()) {
            break;
        }

        char op = expression[pos];

        if (op != '+' && op != '-') {
            break;
        }

        ++pos;

        float rhs = parse_term(expression, pos);

        if (op == '+') {
            value += rhs;
        } else {
            value -= rhs;
        }
    }

    return value;
}

float evaluate_expression(const std::string& expression) {
    size_t pos = 0;

    float result = parse_expression(expression, pos);

    skip_whitespace(expression, pos);

    if (pos != expression.size()) {
        throw std::runtime_error("Unexpected character: " + std::string(1, expression[pos]));
    }

    return result;
}
