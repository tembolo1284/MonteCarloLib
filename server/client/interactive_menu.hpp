#ifndef INTERACTIVE_MENU_HPP
#define INTERACTIVE_MENU_HPP

#include "client_core.hpp"
#include "option_pricers.hpp"
#include "input_utils.hpp"
#include <iostream>

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_RED     "\033[31m"
#define COLOR_CYAN    "\033[36m"

namespace interactive {

inline void print_menu() {
    std::cout << COLOR_CYAN << "Available Options:" << COLOR_RESET << std::endl;
    std::cout << "  1. Price European Call" << std::endl;
    std::cout << "  2. Price European Put" << std::endl;
    std::cout << "  3. Price American Call" << std::endl;
    std::cout << "  4. Price American Put" << std::endl;
    std::cout << "  5. Price Asian Call" << std::endl;
    std::cout << "  6. Quick Test (ATM European Call)" << std::endl;
    std::cout << "  0. Exit" << std::endl;
    std::cout << std::endl;
}

inline void run_interactive_mode(McOptionsClient& client) {
    std::cout << COLOR_BLUE << "============================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_BLUE << "  Monte Carlo Options Pricing Client" << COLOR_RESET << std::endl;
    std::cout << COLOR_BLUE << "============================================" << COLOR_RESET << std::endl;
    std::cout << "Connected to: " << COLOR_GREEN << client.get_server_address() << COLOR_RESET << std::endl;
    std::cout << std::endl;
    
    while (true) {
        print_menu();
        int choice = input_utils::get_menu_choice();
        
        switch (choice) {
            case 1:
                pricers::price_european_call(client);
                break;
            case 2:
                pricers::price_european_put(client);
                break;
            case 3:
                pricers::price_american_call(client);
                break;
            case 4:
                pricers::price_american_put(client);
                break;
            case 5:
                pricers::price_asian_call(client);
                break;
            case 6:
                pricers::quick_test(client);
                break;
            case 0:
                std::cout << std::endl << COLOR_GREEN << "Goodbye!" << COLOR_RESET << std::endl;
                return;
            default:
                std::cout << COLOR_RED << "Invalid choice. Try again." << COLOR_RESET << std::endl;
        }
        
        std::cout << std::endl;
    }
}

} // namespace interactive

#endif
