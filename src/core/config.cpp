#include "core/config.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <toml++/toml.hpp>

namespace config {

std::expected<void, Error> check_and_create_config_dir() {
    const char* home = std::getenv("HOME");

    if (home == nullptr) {
        return std::unexpected("HOME is not set");
    }

    const std::filesystem::path CONFIG_DIRECTORY = std::filesystem::path(home) / ".config" / "cdsl";

    std::error_code ecd;

    std::filesystem::create_directories(CONFIG_DIRECTORY, ecd);

    if (ecd) {
        return std::unexpected("Failed to create config directory: " + ecd.message());
    }

    const std::filesystem::path CONFIG_FILE = CONFIG_DIRECTORY / "config.toml";

    if (!std::filesystem::exists(CONFIG_FILE, ecd)) {
        if (ecd) {
            return std::unexpected("Failed to check config file: " + ecd.message());
        }

        std::ofstream file(CONFIG_FILE);

        if (!file) {
            return std::unexpected("Failed to create config file: " + CONFIG_FILE.string());
        }
    }

    return {};
}

std::expected<toml::table, Error> load_config() {
    const char* home = std::getenv("HOME");

    if (home == nullptr) {
        return std::unexpected("HOME is not set");
    }

    const auto CONFIG_FILE = std::filesystem::path(home) / ".config" / "cdsl" / "config.toml";

    try {
        return toml::parse_file(CONFIG_FILE.string());
    } catch (const toml::parse_error& error) {
        return std::unexpected("Failed to parse config: " + std::string(error.description()));
    }
}

std::expected<bool, Error> component_exists(const std::string& component_name) {
    auto config = load_config();

    if (!config) {
        return std::unexpected(config.error());
    }

    const auto* components = (*config)["component"].as_table();

    if (components == nullptr) {
        return false;
    }

    return components->contains(component_name);
}

std::expected<uint8_t, Error> component_id(const std::string& component_name) {
    auto config = load_config();

    if (!config) {
        return std::unexpected(config.error());
    }

    const auto* components = (*config)["component"].as_table();

    if (components == nullptr) {
        return std::unexpected("No components configured");
    }

    const auto* component = (*components)[component_name].as_table();

    if (component == nullptr) {
        return std::unexpected("Component not configured: " + component_name);
    }

    const auto COMPONENT_ID = (*component)["id"].value<int64_t>();

    if (!COMPONENT_ID) {
        return std::unexpected("Component ID is not configured: " + component_name);
    }

    if (*COMPONENT_ID < 1 || *COMPONENT_ID > 255) {
        return std::unexpected("Component ID must be between 1 and 255");
    }

    return static_cast<uint8_t>(*COMPONENT_ID);
}

std::expected<bool, Error> value_within_limits(const std::string& component_name, double value) {
    auto config = load_config();

    if (!config) {
        return std::unexpected(config.error());
    }

    const auto* components = (*config)["component"].as_table();

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

std::expected<std::string, Error> arduino_port() {
    auto config = load_config();

    if (!config) {
        return std::unexpected(config.error());
    }

    const auto PORT = (*config)["arduino"]["port"].value<std::string>();

    if (!PORT) {
        return std::unexpected("Arduino serial port is not configured");
    }

    return *PORT;
}

std::expected<int, Error> arduino_baud_rate() {
    auto config = load_config();

    if (!config) {
        return std::unexpected(config.error());
    }

    const auto BAUD_RATE = (*config)["arduino"]["baud_rate"].value<int64_t>();

    if (!BAUD_RATE) {
        return std::unexpected("Arduino baud rate is not configured");
    }

    return static_cast<int>(*BAUD_RATE);
}

}  // namespace config
