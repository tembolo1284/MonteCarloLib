#ifndef INPUT_UTILS_HPP
#define INPUT_UTILS_HPP

#include <iostream>
#include <string>
#include <limits>

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_CYAN    "\033[36m"

namespace input_utils {

inline double get_input(const std::string& prompt, double default_value) {
    std::cout << "  " << prompt << " [" << default_value << "]: ";
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) {
        return default_value;
    }
    
    try {
        return std::stod(input);
    } catch (...) {
        std::cout << COLOR_RED << "  Invalid input, using default: " << default_value << COLOR_RESET << std::endl;
        return default_value;
    }
}

inline int get_int_input(const std::string& prompt, int default_value) {
    std::cout << "  " << prompt << " [" << default_value << "]: ";
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) {
        return default_value;
    }
    
    try {
        return std::stoi(input);
    } catch (...) {
        std::cout << COLOR_RED << "  Invalid input, using default: " << default_value << COLOR_RESET << std::endl;
        return default_value;
    }
}

inline bool get_bool_input(const std::string& prompt, bool default_value) {
    std::cout << "  " << prompt << " (y/n) [" << (default_value ? "y" : "n") << "]: ";
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) {
        return default_value;
    }
    
    return (input == "y" || input == "Y" || input == "yes" || input == "Yes");
}

inline mcoptions::SimulationConfig get_simulation_config() {
    mcoptions::SimulationConfig config;
    
    std::cout << std::endl << COLOR_CYAN << "Simulation Configuration:" << COLOR_RESET << std::endl;
    config.set_num_simulations(get_int_input("Number of simulations", 100000));
    config.set_num_steps(get_int_input("Number of time steps", 252));
    config.set_antithetic_enabled(get_bool_input("Enable antithetic variates", true));
    config.set_control_variates_enabled(get_bool_input("Enable control variates", true));
    config.set_stratified_sampling_enabled(get_bool_input("Enable stratified sampling", false));
    
    return config;
}

inline int get_menu_choice() {
    std::cout << "Enter your choice: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}

} // namespace input_utils

#endif
