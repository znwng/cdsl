#include <iostream>
#include <string_view>

#include "core/runtime.hpp"

int main(int argc, char* argv[]) {
    if (argc > 1) {
        if (argc == 2 && std::string_view(argv[1]) == "--help") {
            std::cout << "Usage: cdsl [--help]\n";
            return 0;
        }
        std::cerr << "Error: Unknown option.\n";
        std::cerr << "Usage: cdsl [--help]\n";
        return 1;
    }
    run_interactive_mode();
    return 0;
}
