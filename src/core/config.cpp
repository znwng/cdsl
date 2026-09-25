#include "core/config.hpp"

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <limits>
#include <string>
#include <toml++/toml.hpp>
#include <utility>

namespace config {

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

std::expected<bool, Error> component_exists(const toml::table& config, const std::string& component_name) {
    const auto* components = config["component"].as_table();

    if (components == nullptr) {
        return false;
    }

    return components->contains(component_name);
}

std::expected<uint8_t, Error> component_id(const toml::table& config, const std::string& component_name) {
    const auto* components = config["component"].as_table();

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

    // Component id should be in between 0 and 255 both included
    if (std::cmp_less(*COMPONENT_ID, std::numeric_limits<uint8_t>::min()) ||
        std::cmp_greater(*COMPONENT_ID, std::numeric_limits<uint8_t>::max())) {
        return std::unexpected("Component ID must be between 0 and 255");
    }

    return static_cast<uint8_t>(*COMPONENT_ID);
}

std::expected<bool, Error> value_within_limits(const toml::table& config, const std::string& component_name,
                                               double value) {
    const auto* components = config["component"].as_table();

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

std::expected<std::string, Error> arduino_port(const toml::table& config) {
    const auto PORT = config["arduino"]["port"].value<std::string>();

    if (!PORT) {
        return std::unexpected("Arduino serial port is not configured");
    }

    return *PORT;
}

std::expected<int, Error> arduino_baud_rate(const toml::table& config) {
    const auto BAUD_RATE = config["arduino"]["baud_rate"].value<int64_t>();

    if (!BAUD_RATE) {
        return std::unexpected("Arduino baud rate is not configured");
    }

    return static_cast<int>(*BAUD_RATE);
}

}  // namespace config
