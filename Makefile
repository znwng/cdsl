.PHONY: all setup build clean rebuild check-deps

BUILD_DIR := build
BUILD_TYPE ?= Debug

all: build

check-deps:
	@command -v cmake >/dev/null 2>&1 || \
		{ echo "Error: CMake is not installed."; exit 1; }
	@command -v ninja >/dev/null 2>&1 || \
		{ echo "Error: Ninja is not installed."; exit 1; }
	@command -v git >/dev/null 2>&1 || \
		{ echo "Error: Git is not installed."; exit 1; }
	@command -v c++ >/dev/null 2>&1 || \
		{ echo "Error: C++ compiler is not installed."; exit 1; }

setup: check-deps
	git submodule update --init --recursive
	cmake -S . -B $(BUILD_DIR) \
		-G Ninja \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	ln -sf $(BUILD_DIR)/compile_commands.json compile_commands.json

build:
	@if [ ! -d "$(BUILD_DIR)" ]; then $(MAKE) setup; fi
	cmake --build $(BUILD_DIR) -j$$(nproc)

clean:
	rm -rf $(BUILD_DIR) compile_commands.json

rebuild: clean setup build
