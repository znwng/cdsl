.PHONY: all check-deps setup build clean rebuild format

BUILD_DIR := build
BUILD_TYPE ?= Debug

all: build

check-deps:
	@command -v cmake >/dev/null 2>&1 || { \
		echo "Error: CMake is not installed."; \
		echo "Install it with: sudo apt update && sudo apt install cmake"; \
		exit 1; \
	}

	@command -v ninja >/dev/null 2>&1 || { \
		echo "Error: Ninja is not installed."; \
		echo "Install it with: sudo apt update && sudo apt install ninja-build"; \
		exit 1; \
	}

	@command -v c++ >/dev/null 2>&1 || { \
		echo "Error: A C++ compiler is not installed."; \
		echo "Install it with: sudo apt update && sudo apt install build-essential"; \
		exit 1; \
	}

	@command -v git >/dev/null 2>&1 || { \
		echo "Error: Git is not installed."; \
		echo "Install it with: sudo apt update && sudo apt install git"; \
		exit 1; \
	}

setup: check-deps
	@echo "Initializing Git submodules..."
	git submodule update --init --recursive

	@echo "Configuring CDSL..."
	cmake -S . \
		-B $(BUILD_DIR) \
		-G Ninja \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

	@ln -sf $(BUILD_DIR)/compile_commands.json compile_commands.json

build: setup
	@echo "Building CDSL..."
	cmake --build $(BUILD_DIR) --parallel

format:
	@command -v clang-format >/dev/null 2>&1 || { \
		echo "Error: clang-format is not installed."; \
		echo "Install it with: sudo apt update && sudo apt install clang-format"; \
		exit 1; \
	}

	find src include -type f \( \
		-name '*.c' -o \
		-name '*.h' -o \
		-name '*.cpp' -o \
		-name '*.hpp' \
	\) -exec clang-format -i {} +

clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR) compile_commands.json

rebuild: clean
	$(MAKE) build
