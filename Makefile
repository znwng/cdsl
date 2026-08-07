.PHONY: all setup build clean rebuild

BUILD_DIR := build
BUILD_TYPE ?= Debug

all: build

setup:
	git submodule update --init --recursive
	cmake -S . -B $(BUILD_DIR) \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	ln -sf $(BUILD_DIR)/compile_commands.json compile_commands.json

build:
	@if [ ! -d "$(BUILD_DIR)" ]; then $(MAKE) setup; fi
	cmake --build $(BUILD_DIR) -j$(shell nproc)

clean:
	rm -rf $(BUILD_DIR) compile_commands.json

rebuild: clean setup build
