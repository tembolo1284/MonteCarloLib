#ifndef OPTION_PRICERS_HPP
#define OPTION_PRICERS_HPP

#include "client_core.hpp"
#include "input_utils.hpp"
#include <iostream>
#include <iomanip>

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"
#define COLOR_CYAN    "\033[36m"

namespace pricers {

inline void price_european_call(McOptionsClient& client) {
    std::cout << std::endl << COLOR_GREEN << "=== European Call Option ===" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Example: ATM call on stock trading at $100" << COLOR_RESET << std::endl;
    std::cout << std::endl;
    
    mcoptions::EuropeanRequest request;
    request.set_spot(input_utils::get_input("Spot price (S)", 100.0));
    request.set_strike(input_utils::get_input("Strike price (K)", 100.0));
    request.set_rate(input_utils::get_input("Risk-free rate (r)", 0.05));
    request.set_volatility(input_utils::get_input("Volatility (σ)", 0.20));
    request.set_time_to_maturity(input_utils::get_input("Time to maturity (T) in years", 1.0));
    
    *request.mutable_config() = input_utils::get_simulation_config();
    
    mcoptions::PriceResponse response;
    ClientContext context;
    
    std::cout << std::endl << COLOR_BLUE << "Sending request to server..." << COLOR_RESET << std::endl;
    Status status = client.get_stub()->PriceEuropeanCall(&context, request, &response);
    
    if (status.ok()) {
        std::cout << std::endl << COLOR_GREEN << "✓ Result:" << COLOR_RESET << std::endl;
        std::cout << "  Price: " << COLOR_YELLOW << "$" << std::fixed << std::setprecision(4)
                 << response.price() << COLOR_RESET << std::endl;
        std::cout << "  Computation time: " << COLOR_CYAN << response.computation_time_ms() 
                 << "ms" << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_RED << "✗ RPC failed: " << status.error_message() << COLOR_RESET << std::endl;
    }
}

inline void price_european_put(McOptionsClient& client) {
    std::cout << std::endl << COLOR_GREEN << "=== European Put Option ===" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Example: ATM put on stock trading at $100" << COLOR_RESET << std::endl;
    std::cout << std::endl;
    
    mcoptions::EuropeanRequest request;
    request.set_spot(input_utils::get_input("Spot price (S)", 100.0));
    request.set_strike(input_utils::get_input("Strike price (K)", 100.0));
    request.set_rate(input_utils::get_input("Risk-free rate (r)", 0.05));
    request.set_volatility(input_utils::get_input("Volatility (σ)", 0.20));
    request.set_time_to_maturity(input_utils::get_input("Time to maturity (T) in years", 1.0));
    
    *request.mutable_config() = input_utils::get_simulation_config();
    
    mcoptions::PriceResponse response;
    ClientContext context;
    
    std::cout << std::endl << COLOR_BLUE << "Sending request to server..." << COLOR_RESET << std::endl;
    Status status = client.get_stub()->PriceEuropeanPut(&context, request, &response);
    
    if (status.ok()) {
        std::cout << std::endl << COLOR_GREEN << "✓ Result:" << COLOR_RESET << std::endl;
        std::cout << "  Price: " << COLOR_YELLOW << "$" << std::fixed << std::setprecision(4)
                 << response.price() << COLOR_RESET << std::endl;
        std::cout << "  Computation time: " << COLOR_CYAN << response.computation_time_ms() 
                 << "ms" << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_RED << "✗ RPC failed: " << status.error_message() << COLOR_RESET << std::endl;
    }
}

inline void price_american_call(McOptionsClient& client) {
    std::cout << std::endl << COLOR_GREEN << "=== American Call Option ===" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Example: ITM call with early exercise" << COLOR_RESET << std::endl;
    std::cout << std::endl;
    
    mcoptions::AmericanRequest request;
    request.set_spot(input_utils::get_input("Spot price (S)", 110.0));
    request.set_strike(input_utils::get_input("Strike price (K)", 100.0));
    request.set_rate(input_utils::get_input("Risk-free rate (r)", 0.05));
    request.set_volatility(input_utils::get_input("Volatility (σ)", 0.20));
    request.set_time_to_maturity(input_utils::get_input("Time to maturity (T) in years", 1.0));
    request.set_num_exercise_points(input_utils::get_int_input("Number of exercise points", 50));
    
    *request.mutable_config() = input_utils::get_simulation_config();
    
    mcoptions::PriceResponse response;
    ClientContext context;
    
    std::cout << std::endl << COLOR_BLUE << "Sending request to server..." << COLOR_RESET << std::endl;
    Status status = client.get_stub()->PriceAmericanCall(&context, request, &response);
    
    if (status.ok()) {
        std::cout << std::endl << COLOR_GREEN << "✓ Result:" << COLOR_RESET << std::endl;
        std::cout << "  Price: " << COLOR_YELLOW << "$" << std::fixed << std::setprecision(4)
                 << response.price() << COLOR_RESET << std::endl;
        std::cout << "  Computation time: " << COLOR_CYAN << response.computation_time_ms() 
                 << "ms" << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_RED << "✗ RPC failed: " << status.error_message() << COLOR_RESET << std::endl;
    }
}

inline void price_american_put(McOptionsClient& client) {
    std::cout << std::endl << COLOR_GREEN << "=== American Put Option ===" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Example: ITM put with early exercise" << COLOR_RESET << std::endl;
    std::cout << std::endl;
    
    mcoptions::AmericanRequest request;
    request.set_spot(input_utils::get_input("Spot price (S)", 90.0));
    request.set_strike(input_utils::get_input("Strike price (K)", 100.0));
    request.set_rate(input_utils::get_input("Risk-free rate (r)", 0.05));
    request.set_volatility(input_utils::get_input("Volatility (σ)", 0.20));
    request.set_time_to_maturity(input_utils::get_input("Time to maturity (T) in years", 1.0));
    request.set_num_exercise_points(input_utils::get_int_input("Number of exercise points", 50));
    
    *request.mutable_config() = input_utils::get_simulation_config();
    
    mcoptions::PriceResponse response;
    ClientContext context;
    
    std::cout << std::endl << COLOR_BLUE << "Sending request to server..." << COLOR_RESET << std::endl;
    Status status = client.get_stub()->PriceAmericanPut(&context, request, &response);
    
    if (status.ok()) {
        std::cout << std::endl << COLOR_GREEN << "✓ Result:" << COLOR_RESET << std::endl;
        std::cout << "  Price: " << COLOR_YELLOW << "$" << std::fixed << std::setprecision(4)
                 << response.price() << COLOR_RESET << std::endl;
        std::cout << "  Computation time: " << COLOR_CYAN << response.computation_time_ms() 
                 << "ms" << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_RED << "✗ RPC failed: " << status.error_message() << COLOR_RESET << std::endl;
    }
}

inline void price_asian_call(McOptionsClient& client) {
    std::cout << std::endl << COLOR_GREEN << "=== Asian Call Option ===" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << "Example: Monthly observations over 1 year" << COLOR_RESET << std::endl;
    std::cout << std::endl;
    
    mcoptions::AsianRequest request;
    request.set_spot(input_utils::get_input("Spot price (S)", 100.0));
    request.set_strike(input_utils::get_input("Strike price (K)", 100.0));
    request.set_rate(input_utils::get_input("Risk-free rate (r)", 0.05));
    request.set_volatility(input_utils::get_input("Volatility (σ)", 0.20));
    request.set_time_to_maturity(input_utils::get_input("Time to maturity (T) in years", 1.0));
    request.set_num_observations(input_utils::get_int_input("Number of observations", 12));
    
    *request.mutable_config() = input_utils::get_simulation_config();
    
    mcoptions::PriceResponse response;
    ClientContext context;
    
    std::cout << std::endl << COLOR_BLUE << "Sending request to server..." << COLOR_RESET << std::endl;
    Status status = client.get_stub()->PriceAsianCall(&context, request, &response);
    
    if (status.ok()) {
        std::cout << std::endl << COLOR_GREEN << "✓ Result:" << COLOR_RESET << std::endl;
        std::cout << "  Price: " << COLOR_YELLOW << "$" << std::fixed << std::setprecision(4)
                 << response.price() << COLOR_RESET << std::endl;
        std::cout << "  Computation time: " << COLOR_CYAN << response.computation_time_ms() 
                 << "ms" << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_RED << "✗ RPC failed: " << status.error_message() << COLOR_RESET << std::endl;
    }
}

inline void quick_test(McOptionsClient& client) {
    std::cout << std::endl << COLOR_GREEN << "=== Quick Test (ATM European Call) ===" << COLOR_RESET << std::endl;
    
    mcoptions::EuropeanRequest request;
    request.set_spot(100.0);
    request.set_strike(100.0);
    request.set_rate(0.05);
    request.set_volatility(0.20);
    request.set_time_to_maturity(1.0);
    
    auto* config = request.mutable_config();
    config->set_num_simulations(100000);
    config->set_num_steps(252);
    config->set_antithetic_enabled(true);
    config->set_control_variates_enabled(true);
    
    mcoptions::PriceResponse response;
    ClientContext context;
    
    Status status = client.get_stub()->PriceEuropeanCall(&context, request, &response);
    
    if (status.ok()) {
        std::cout << "  " << COLOR_GREEN << "✓ Price: $" << std::fixed << std::setprecision(4)
                 << response.price() << " (" << response.computation_time_ms() << "ms)" 
                 << COLOR_RESET << std::endl;
    } else {
        std::cout << "  " << COLOR_RED << "✗ Failed: " << status.error_message() << COLOR_RESET << std::endl;
    }
}

} // namespace pricers

#endif
