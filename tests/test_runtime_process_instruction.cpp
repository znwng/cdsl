#include <gtest/gtest.h>

#include "../include/constants.hpp"
#include "../include/runtime.hpp"
#include "../include/types.hpp"

class ProcessInstructionTest : public ::testing::Test {
protected:
    void SetUp() override { clear_constants(); }

    void TearDown() override { clear_constants(); }
};

// ---------------------------------------------------------
// SET
// ---------------------------------------------------------

TEST_F(ProcessInstructionTest, SetLiteralFloat) {
    Instruction instruction{"SET", "speed", "10.5"};

    process_instruction(instruction, 1, true);

    EXPECT_TRUE(has_constant("speed"));
    EXPECT_FLOAT_EQ(get_constant("speed"), 10.5f);
}

TEST_F(ProcessInstructionTest, SetExpression) {
    Instruction instruction{"SET", "speed", "#[2+3*4]"};

    process_instruction(instruction, 1, true);

    EXPECT_TRUE(has_constant("speed"));
    EXPECT_FLOAT_EQ(get_constant("speed"), 14.0f);
}

TEST_F(ProcessInstructionTest, SetInvalidFloatDoesNotCreateConstant) {
    Instruction instruction{"SET", "speed", "abc"};

    process_instruction(instruction, 1, true);

    EXPECT_FALSE(has_constant("speed"));
}

TEST_F(ProcessInstructionTest, SetInvalidExpressionDoesNotCreateConstant) {
    Instruction instruction{"SET", "speed", "#[2+*3]"};

    process_instruction(instruction, 1, true);

    EXPECT_FALSE(has_constant("speed"));
}

TEST_F(ProcessInstructionTest, SetInvalidArgumentCount) {
    Instruction instruction{"SET", "speed"};

    process_instruction(instruction, 1, true);

    EXPECT_FALSE(has_constant("speed"));
}

// ---------------------------------------------------------
// PRINT
// ---------------------------------------------------------

TEST_F(ProcessInstructionTest, PrintConstant) {
    set_constant("speed", 42.0f);

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

TEST_F(ProcessInstructionTest, PrintUnknownConstant) {
    Instruction instruction{"PRINT", "$does_not_exist"};

    testing::internal::CaptureStdout();

    process_instruction(instruction, 1, true);

    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("No constant with name does_not_exist"), std::string::npos);
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

TEST_F(ProcessInstructionTest, WaitConstant) {
    set_constant("delay", 100.0f);

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
