#include <gtest/gtest.h>

#include "../include/validation.hpp"

// =========================================================
// is_valid_float_value
// =========================================================

TEST(ValidationTest, ValidFloat) {
    auto result = is_valid_float_value("10.5");

    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(*result, 10.5f);
}

TEST(ValidationTest, ValidIntegerAsFloat) {
    auto result = is_valid_float_value("42");

    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(*result, 42.0f);
}

TEST(ValidationTest, ValidNegativeFloat) {
    auto result = is_valid_float_value("-12.75");

    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(*result, -12.75f);
}

TEST(ValidationTest, ValidPositiveFloat) {
    auto result = is_valid_float_value("+12.75");

    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(*result, 12.75f);
}

TEST(ValidationTest, ValidScientificNotation) {
    auto result = is_valid_float_value("1.5e3");

    ASSERT_TRUE(result.has_value());
    EXPECT_FLOAT_EQ(*result, 1500.0f);
}

TEST(ValidationTest, InvalidFloat) {
    auto result = is_valid_float_value("abc");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "Invalid value: abc");
}

TEST(ValidationTest, FloatWithTrailingCharacters) {
    auto result = is_valid_float_value("10.5abc");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "Invalid value: 10.5abc");
}

TEST(ValidationTest, EmptyFloat) {
    auto result = is_valid_float_value("");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "Invalid value: ");
}

TEST(ValidationTest, FloatOutOfRange) {
    auto result = is_valid_float_value("999999999999999999999999999999999999999999999999999");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "Value out of range");
}

// =========================================================
// is_valid_int_value
// =========================================================

TEST(ValidationTest, ValidInteger) {
    auto result = is_valid_int_value("42");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 42);
}

TEST(ValidationTest, ValidNegativeInteger) {
    auto result = is_valid_int_value("-42");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, -42);
}

TEST(ValidationTest, ValidPositiveInteger) {
    auto result = is_valid_int_value("+42");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 42);
}

TEST(ValidationTest, ValidZero) {
    auto result = is_valid_int_value("0");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0);
}

TEST(ValidationTest, InvalidInteger) {
    auto result = is_valid_int_value("abc");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "Invalid value: abc");
}

TEST(ValidationTest, IntegerWithDecimal) {
    auto result = is_valid_int_value("10.5");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "Invalid value: 10.5");
}

TEST(ValidationTest, IntegerWithTrailingCharacters) {
    auto result = is_valid_int_value("42abc");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "Invalid value: 42abc");
}

TEST(ValidationTest, EmptyInteger) {
    auto result = is_valid_int_value("");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "Invalid value: ");
}

TEST(ValidationTest, IntegerOutOfRange) {
    auto result = is_valid_int_value("999999999999999999999999999999999999");

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "Value out of range");
}

// =========================================================
// is_valid_variable_name
// =========================================================

TEST(ValidationTest, ValidVariableName) { EXPECT_TRUE(is_valid_variable_name("speed")); }

TEST(ValidationTest, ValidVariableNameWithDigits) { EXPECT_TRUE(is_valid_variable_name("speed123")); }

TEST(ValidationTest, ValidVariableNameWithUnderscore) { EXPECT_TRUE(is_valid_variable_name("motor_speed")); }

TEST(ValidationTest, ValidVariableNameStartingWithUnderscore) { EXPECT_TRUE(is_valid_variable_name("_speed")); }

TEST(ValidationTest, ValidVariableNameWithOnlyUnderscore) { EXPECT_TRUE(is_valid_variable_name("_")); }

TEST(ValidationTest, InvalidVariableNameStartingWithDigit) { EXPECT_FALSE(is_valid_variable_name("123speed")); }

TEST(ValidationTest, InvalidVariableNameStartingWithSpecialCharacter) {
    EXPECT_FALSE(is_valid_variable_name("@speed"));
}

TEST(ValidationTest, InvalidVariableNameWithHyphen) { EXPECT_FALSE(is_valid_variable_name("motor-speed")); }

TEST(ValidationTest, InvalidVariableNameWithSpace) { EXPECT_FALSE(is_valid_variable_name("motor speed")); }

TEST(ValidationTest, InvalidEmptyVariableName) { EXPECT_FALSE(is_valid_variable_name("")); }
