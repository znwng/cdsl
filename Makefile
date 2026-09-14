.PHONY: all clean rebuild check install

BUILD_DIR := build
BUILD_TYPE ?= Debug
INSTALL_DIR := $(HOME)/.local/bin
BINARY := cdsl

all: $(BUILD_DIR)/$(BINARY)

check:
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

$(BUILD_DIR)/build.ninja: check
	@echo "Initializing Git submodules..."
	git submodule update --init --recursive

	@echo "Configuring CDSL..."
	cmake -S . \
		-B $(BUILD_DIR) \
		-G Ninja \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

	@ln -sf $(BUILD_DIR)/compile_commands.json compile_commands.json

$(BUILD_DIR)/$(BINARY): $(BUILD_DIR)/build.ninja
	@echo "Building CDSL..."
	cmake --build $(BUILD_DIR) --parallel

clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR) compile_commands.json

rebuild: clean
	$(MAKE)

install: $(BUILD_DIR)/$(BINARY)
	@echo "Installing CDSL to $(INSTALL_DIR)..."
	@mkdir -p $(INSTALL_DIR)
	@install -m 755 $(BUILD_DIR)/$(BINARY) $(INSTALL_DIR)/$(BINARY)

uninstall:
	@echo "Removing CDSL from $(INSTALL_DIR)..."
	@rm -f $(INSTALL_DIR)/$(BINARY)
