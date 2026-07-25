CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++23

SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
TARGET := $(BUILD_DIR)/cdsl

SRC := $(wildcard $(SRC_DIR)/*.cpp)

.PHONY: all format run clean

all: format $(TARGET)

format:
	clang-format -i $(SRC_DIR)/*.cpp $(INCLUDE_DIR)/*.hpp

$(TARGET): $(SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

