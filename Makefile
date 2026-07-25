CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++23

SRC := src/main.cpp
BUILD_DIR := build
TARGET := $(BUILD_DIR)/cdsl

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

