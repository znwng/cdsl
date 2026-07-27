CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++23 -Iinclude

SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
TARGET := $(BUILD_DIR)/cdsl

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC))

.PHONY: all format run clean

all: format $(TARGET)

format:
	clang-format -i $(SRC_DIR)/*.cpp $(INCLUDE_DIR)/*.hpp

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

