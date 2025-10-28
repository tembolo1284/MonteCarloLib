#ifndef MCOPTIONS_LOGGING_HPP
#define MCOPTIONS_LOGGING_HPP

#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include "mcoptions.grpc.pb.h"

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_CYAN    "\033[36m"

namespace mcoptions {
namespace logging {

inline std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

inline std::string format_config(const SimulationConfig& config) {
    std::stringstream ss;
    ss << "Sims: " << config.num_simulations() 
       << ", Steps: " << config.num_steps();
    if (config.antithetic_enabled() || config.control_variates_enabled() || config.stratified_sampling_enabled()) {
        ss << " [";
        bool first = true;
        if (config.antithetic_enabled()) { ss << "AV"; first = false; }
        if (config.control_variates_enabled()) { if (!first) ss << ","; ss << "CV"; first = false; }
        if (config.stratified_sampling_enabled()) { if (!first) ss << ","; ss << "SS"; }
        ss << "]";
    }
    return ss.str();
}

inline void log_request(const std::string& method, const std::string& params) {
    std::cout << COLOR_CYAN << "[" << get_timestamp() << "] " 
              << COLOR_RESET << "Request: " << COLOR_GREEN << method << COLOR_RESET << std::endl;
    std::cout << "  " << params << std::endl;
}

inline void log_result(double price, long duration_ms) {
    std::cout << "  " << COLOR_YELLOW << "Result: $" << std::fixed << std::setprecision(4) 
              << price << COLOR_RESET 
              << COLOR_BLUE << " (computed in " << duration_ms << "ms)" 
              << COLOR_RESET << std::endl << std::endl;
}

inline void log_batch_complete(long duration_ms) {
    std::cout << "  " << COLOR_YELLOW << "Batch completed" << COLOR_RESET 
              << COLOR_BLUE << " (total time: " << duration_ms << "ms)" 
              << COLOR_RESET << std::endl << std::endl;
}

} // namespace logging
} // namespace mcoptions

#endif
