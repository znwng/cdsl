#include "core/config.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <toml++/toml.hpp>

namespace config {

void check_and_create_config_dir() {
    const char* home = std::getenv("HOME");

    if (home == nullptr) {
        throw std::runtime_error("HOME is not set");
    }

    const std::filesystem::path CONFIG_DIRECTORY = std::filesystem::path(home) / ".config" / "cdsl";

    std::filesystem::create_directories(CONFIG_DIRECTORY);

    const std::filesystem::path CONFIG_FILE = CONFIG_DIRECTORY / "config.toml";

    if (!std::filesystem::exists(CONFIG_FILE)) {
        std::ofstream{CONFIG_FILE};
    }
}

toml::table load_config() {
    const char* home = std::getenv("HOME");

    if (home == nullptr) {
        throw std::runtime_error("HOME is not set");
    }

    const auto CONFIG_FILE = std::filesystem::path(home) / ".config" / "cdsl" / "config.toml";

    return toml::parse_file(CONFIG_FILE.string());
}

bool component_exists(const std::string& component_name) {
    const auto CONFIG = load_config();

    const auto* components = CONFIG["component"].as_table();

    if (components == nullptr) {
        return false;
    }

    return components->contains(component_name);
}

uint8_t component_id(const std::string& component_name) {
    const auto CONFIG = load_config();

    const auto* components = CONFIG["component"].as_table();

    if (components == nullptr) {
        throw std::runtime_error("No components configured");
    }

    const auto* component = (*components)[component_name].as_table();

    if (component == nullptr) {
        throw std::runtime_error("Component not configured: " + component_name);
    }

    const auto COMPONENT_ID = (*component)["id"].value<int64_t>();

    if (!COMPONENT_ID) {
        throw std::runtime_error("Component ID is not configured: " + component_name);
    }

    if (*COMPONENT_ID < 1 || *COMPONENT_ID > 255) {
        throw std::runtime_error("Component ID must be between 1 and 255");
    }

    return static_cast<uint8_t>(*COMPONENT_ID);
}

bool value_within_limits(const std::string& component_name, double value) {
    const auto CONFIG = load_config();

    const auto* components = CONFIG["component"].as_table();

    if (components == nullptr) {
        return false;
    }

    const auto* component = (*components)[component_name].as_table();

    if (component == nullptr) {
        return false;
    }

    const auto MIN = (*component)["min"].value<double>();
    const auto MAX = (*component)["max"].value<double>();

    if (!MIN || !MAX) {
        return false;
    }

    return value >= *MIN && value <= *MAX;
}

std::string arduino_port() {
    const auto CONFIG = load_config();

    const auto PORT = CONFIG["arduino"]["port"].value<std::string>();

    if (!PORT) {
        throw std::runtime_error("Arduino serial port is not configured");
    }

    return *PORT;
}

int arduino_baud_rate() {
    const auto CONFIG = load_config();

    const auto BAUD_RATE = CONFIG["arduino"]["baud_rate"].value<int64_t>();

    if (!BAUD_RATE) {
        throw std::runtime_error("Arduino baud rate is not configured");
    }

    return static_cast<int>(*BAUD_RATE);
}

}  // namespace config
