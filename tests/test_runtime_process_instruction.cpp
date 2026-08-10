#include <gtest/gtest.h>

#include "../include/variables.hpp"
#include "../include/runtime.hpp"
#include "../include/types.hpp"

class ProcessInstructionTest : public ::testing::Test {
protected:
    void SetUp() override { clear_variables(); }

    void TearDown() override { clear_variables(); }
};

// ---------------------------------------------------------
// SET
// ---------------------------------------------------------

TEST_F(ProcessInstructionTest, SetLiteralFloat) {
    Instruction instruction{"SET", "speed", "10.5"};

    process_instruction(instruction, 1, true);

    EXPECT_TRUE(has_variable("speed"));
    EXPECT_FLOAT_EQ(get_variable("speed"), 10.5f);
}

TEST_F(ProcessInstructionTest, SetExpression) {
    Instruction instruction{"SET", "speed", "#[2+3*4]"};

    process_instruction(instruction, 1, true);

    EXPECT_TRUE(has_variable("speed"));
    EXPECT_FLOAT_EQ(get_variable("speed"), 14.0f);
}

TEST_F(ProcessInstructionTest, SetInvalidFloatDoesNotCreatevariable) {
    Instruction instruction{"SET", "speed", "abc"};

    process_instruction(instruction, 1, true);

    EXPECT_FALSE(has_variable("speed"));
}

TEST_F(ProcessInstructionTest, SetInvalidExpressionDoesNotCreatevariable) {
    Instruction instruction{"SET", "speed", "#[2+*3]"};

    process_instruction(instruction, 1, true);

    EXPECT_FALSE(has_variable("speed"));
}

TEST_F(ProcessInstructionTest, SetInvalidArgumentCount) {
    Instruction instruction{"SET", "speed"};

    process_instruction(instruction, 1, true);

    EXPECT_FALSE(has_variable("speed"));
}

// ---------------------------------------------------------
// PRINT
// ---------------------------------------------------------

TEST_F(ProcessInstructionTest, Printvariable) {
    set_variable("speed", 42.0f);

    Instruction instruction{"PRINT", "$speed"};

    testing::internal::CaptureStdout();

    process_instruction(instruction, 1, true);

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "42\n");
}

TEST_F(ProcessInstructionTest, PrintExpression) {
    Instruction instruction{"PRINT", "#[2+3*4]"};

    testing::internal::CaptureStdout();

    process_instruction(instruction, 1, true);

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "14\n");
}

TEST_F(ProcessInstructionTest, PrintUnknownvariable) {
    Instruction instruction{"PRINT", "$does_not_exist"};

    testing::internal::CaptureStdout();

    process_instruction(instruction, 1, true);

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("No variable with name does_not_exist"), std::string::npos);
}

// ---------------------------------------------------------
// WAIT
// ---------------------------------------------------------

TEST_F(ProcessInstructionTest, WaitLiteralInteger) {
    Instruction instruction{"WAIT", "100"};

    EXPECT_NO_THROW(process_instruction(instruction, 1, true));
}

TEST_F(ProcessInstructionTest, WaitExpression) {
    Instruction instruction{"WAIT", "#[50+50]"};

    EXPECT_NO_THROW(process_instruction(instruction, 1, true));
}

TEST_F(ProcessInstructionTest, Waitvariable) {
    set_variable("delay", 100.0f);

    Instruction instruction{"WAIT", "$delay"};

    EXPECT_NO_THROW(process_instruction(instruction, 1, true));
}

TEST_F(ProcessInstructionTest, WaitRejectsNegativeDelay) {
    Instruction instruction{"WAIT", "-10"};

    testing::internal::CaptureStdout();

    process_instruction(instruction, 1, true);

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Delay cannot be negative"), std::string::npos);
}

// ---------------------------------------------------------
// INVALID
// ---------------------------------------------------------

TEST_F(ProcessInstructionTest, InvalidAction) {
    Instruction instruction{"BOGUS"};

    testing::internal::CaptureStdout();

    process_instruction(instruction, 1, true);

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Invalid action BOGUS"), std::string::npos);
}
