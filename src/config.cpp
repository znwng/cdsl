#include "config.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <toml++/toml.hpp>

namespace config {

void check_and_create_config_dir() {
    const char* home = std::getenv("HOME");

    if (!home) {
        throw std::runtime_error("HOME is not set");
    }

    const std::filesystem::path config_dir = std::filesystem::path(home) / ".config" / "cdsl";

    std::filesystem::create_directories(config_dir);

    const std::filesystem::path config_file = config_dir / "config.toml";

    if (!std::filesystem::exists(config_file)) {
        std::ofstream{config_file};
    }
}

toml::table load_config() {
    const char* home = std::getenv("HOME");

    if (!home) {
        throw std::runtime_error("HOME is not set");
    }

    const auto config_file = std::filesystem::path(home) / ".config" / "cdsl" / "config.toml";

    return toml::parse_file(config_file.string());
}

bool component_exists(const std::string& component_name) {
    const auto config = load_config();

    const auto* components = config["component"].as_table();

    if (!components) {
        return false;
    }

    return components->contains(component_name);
}

bool value_within_limits(const std::string& component_name, double value) {
    const auto config = load_config();

    const auto* components = config["component"].as_table();

    if (!components) {
        return false;
    }

    const auto* component = (*components)[component_name].as_table();

    if (!component) {
        return false;
    }

    const auto min = (*component)["min"].value<double>();
    const auto max = (*component)["max"].value<double>();

    if (!min || !max) {
        return false;
    }

    return value >= *min && value <= *max;
}

}  // namespace config
