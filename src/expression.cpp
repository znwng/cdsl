#include "expression.hpp"

#include <cctype>
#include <functional>
#include <stdexcept>
#include <string>

#include "variables.hpp"

float evaluate_expression(const std::string& expression) {
    size_t pos = 0;

    auto skip_whitespace = [&]() {
        while (pos < expression.size() && std::isspace(static_cast<unsigned char>(expression[pos]))) {
            ++pos;
        }
    };

    std::function<float()> parse_expression;
    std::function<float()> parse_term;
    std::function<float()> parse_factor;
    std::function<float()> parse_primary;

    parse_primary = [&]() -> float {
        skip_whitespace();

        if (pos >= expression.size()) {
            throw std::runtime_error("Unexpected end of expression");
        }

        // Parenthesized expression
        if (expression[pos] == '(') {
            ++pos;

            float value = parse_expression();

            skip_whitespace();

            if (pos >= expression.size() || expression[pos] != ')') {
                throw std::runtime_error("Expected ')'");
            }

            ++pos;

            return value;
        }

        // variable: $variable_name
        if (expression[pos] == '$') {
            ++pos;

            size_t start = pos;

            if (pos >= expression.size() ||
                (!std::isalpha(static_cast<unsigned char>(expression[pos])) && expression[pos] != '_')) {
                throw std::runtime_error("Expected variable name after '$'");
            }

            ++pos;

            while (pos < expression.size() &&
                   (std::isalnum(static_cast<unsigned char>(expression[pos])) || expression[pos] == '_')) {
                ++pos;
            }

            std::string name = expression.substr(start, pos - start);

            if (!has_variable(name)) {
                throw std::runtime_error("Undefined variable: $" + name);
            }

            return get_variable(name);
        }

        // Number
        if (std::isdigit(static_cast<unsigned char>(expression[pos])) || expression[pos] == '.') {
            size_t start = pos;

            while (pos < expression.size() &&
                   (std::isdigit(static_cast<unsigned char>(expression[pos])) || expression[pos] == '.')) {
                ++pos;
            }

            return std::stof(expression.substr(start, pos - start));
        }

        throw std::runtime_error("Unexpected character: " + std::string(1, expression[pos]));
    };

    // Unary + and -
    parse_factor = [&]() -> float {
        skip_whitespace();

        if (pos < expression.size() && expression[pos] == '-') {
            ++pos;
            return -parse_factor();
        }

        if (pos < expression.size() && expression[pos] == '+') {
            ++pos;
            return parse_factor();
        }

        return parse_primary();
    };

    // * and /
    parse_term = [&]() -> float {
        float value = parse_factor();

        while (true) {
            skip_whitespace();

            if (pos >= expression.size()) {
                break;
            }

            char op = expression[pos];

            if (op != '*' && op != '/') {
                break;
            }

            ++pos;

            float rhs = parse_factor();

            if (op == '*') {
                value *= rhs;
            } else {
                if (rhs == 0.0f) {
                    throw std::runtime_error("Division by zero");
                }

                value /= rhs;
            }
        }

        return value;
    };

    // + and -
    parse_expression = [&]() -> float {
        float value = parse_term();

        while (true) {
            skip_whitespace();

            if (pos >= expression.size()) {
                break;
            }

            char op = expression[pos];

            if (op != '+' && op != '-') {
                break;
            }

            ++pos;

            float rhs = parse_term();

            if (op == '+') {
                value += rhs;
            } else {
                value -= rhs;
            }
        }

        return value;
    };

    float result = parse_expression();

    skip_whitespace();

    if (pos != expression.size()) {
        throw std::runtime_error("Unexpected character: " + std::string(1, expression[pos]));
    }

    return result;
}
