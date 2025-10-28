#ifndef CLI_PARSER_HPP
#define CLI_PARSER_HPP

#include "client_core.hpp"
#include <iostream>
#include <string>
#include <iomanip>

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_RED     "\033[31m"

namespace cli {

inline void print_usage() {
    std::cout << "Usage: mcoptions_client [SERVER_ADDRESS] [--option-type TYPE] [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "If no options provided, starts interactive mode." << std::endl;
    std::cout << std::endl;
    std::cout << "Option Types:" << std::endl;
    std::cout << "  --european-call        Price a European call option" << std::endl;
    std::cout << "  --european-put         Price a European put option" << std::endl;
    std::cout << "  --american-call        Price an American call option" << std::endl;
    std::cout << "  --american-put         Price an American put option" << std::endl;
    std::cout << "  --asian-call           Price an Asian call option" << std::endl;
    std::cout << std::endl;
    std::cout << "Common Parameters:" << std::endl;
    std::cout << "  --spot S               Spot price (default: 100.0)" << std::endl;
    std::cout << "  --strike K             Strike price (default: 100.0)" << std::endl;
    std::cout << "  --rate r               Risk-free rate (default: 0.05)" << std::endl;
    std::cout << "  --vol σ                Volatility (default: 0.20)" << std::endl;
    std::cout << "  --time T               Time to maturity in years (default: 1.0)" << std::endl;
    std::cout << "  --sims N               Number of simulations (default: 100000)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  mcoptions_client localhost:50051" << std::endl;
    std::cout << "  mcoptions_client --european-call --spot 100 --strike 100 --rate 0.05 --vol 0.2 --time 1.0" << std::endl;
    std::cout << "  mcoptions_client localhost:50052 --american-put --spot 90 --strike 100" << std::endl;
    std::cout << std::endl;
}

struct CliOptions {
    std::string option_type;
    double spot = 100.0;
    double strike = 100.0;
    double rate = 0.05;
    double vol = 0.20;
    double time = 1.0;
    int sims = 100000;
};

inline bool parse_args(int argc, char** argv, int arg_offset, CliOptions& opts) {
    for (int i = arg_offset; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            print_usage();
            return false;
        } else if (arg == "--european-call" || arg == "--european-put" || 
                   arg == "--american-call" || arg == "--american-put" ||
                   arg == "--asian-call") {
            opts.option_type = arg.substr(2);  // Remove "--"
        } else if (arg == "--spot" && i + 1 < argc) {
            opts.spot = std::stod(argv[++i]);
        } else if (arg == "--strike" && i + 1 < argc) {
            opts.strike = std::stod(argv[++i]);
        } else if (arg == "--rate" && i + 1 < argc) {
            opts.rate = std::stod(argv[++i]);
        } else if (arg == "--vol" && i + 1 < argc) {
            opts.vol = std::stod(argv[++i]);
        } else if (arg == "--time" && i + 1 < argc) {
            opts.time = std::stod(argv[++i]);
        } else if (arg == "--sims" && i + 1 < argc) {
            opts.sims = std::stoi(argv[++i]);
        }
    }
    
    if (opts.option_type.empty()) {
        std::cerr << COLOR_RED << "Error: No option type specified" << COLOR_RESET << std::endl;
        std::cerr << "Run with --help for usage information" << std::endl;
        return false;
    }
    
    return true;
}

inline void execute_cli_request(McOptionsClient& client, const CliOptions& opts) {
    std::cout << COLOR_BLUE << "Pricing " << opts.option_type << " option..." << COLOR_RESET << std::endl;
    std::cout << "  S=" << opts.spot << ", K=" << opts.strike << ", r=" << opts.rate 
              << ", σ=" << opts.vol << ", T=" << opts.time << std::endl;
    std::cout << "  Simulations: " << opts.sims << std::endl;
    std::cout << std::endl;
    
    auto stub = client.get_stub();
    
    if (opts.option_type == "european-call") {
        mcoptions::EuropeanRequest request;
        request.set_spot(opts.spot);
        request.set_strike(opts.strike);
        request.set_rate(opts.rate);
        request.set_volatility(opts.vol);
        request.set_time_to_maturity(opts.time);
        request.mutable_config()->set_num_simulations(opts.sims);
        request.mutable_config()->set_num_steps(252);
        request.mutable_config()->set_antithetic_enabled(true);
        
        mcoptions::PriceResponse response;
        ClientContext context;
        Status status = stub->PriceEuropeanCall(&context, request, &response);
        
        if (status.ok()) {
            std::cout << COLOR_GREEN << "Price: $" << std::fixed << std::setprecision(4)
                     << response.price() << COLOR_RESET << std::endl;
            std::cout << "Computation time: " << response.computation_time_ms() << "ms" << std::endl;
        } else {
            std::cerr << COLOR_RED << "RPC failed: " << status.error_message() << COLOR_RESET << std::endl;
        }
    } else if (opts.option_type == "european-put") {
        mcoptions::EuropeanRequest request;
        request.set_spot(opts.spot);
        request.set_strike(opts.strike);
        request.set_rate(opts.rate);
        request.set_volatility(opts.vol);
        request.set_time_to_maturity(opts.time);
        request.mutable_config()->set_num_simulations(opts.sims);
        request.mutable_config()->set_num_steps(252);
        request.mutable_config()->set_antithetic_enabled(true);
        
        mcoptions::PriceResponse response;
        ClientContext context;
        Status status = stub->PriceEuropeanPut(&context, request, &response);
        
        if (status.ok()) {
            std::cout << COLOR_GREEN << "Price: $" << std::fixed << std::setprecision(4)
                     << response.price() << COLOR_RESET << std::endl;
            std::cout << "Computation time: " << response.computation_time_ms() << "ms" << std::endl;
        } else {
            std::cerr << COLOR_RED << "RPC failed: " << status.error_message() << COLOR_RESET << std::endl;
        }
    }
    // Add other option types as needed...
}

} // namespace cli

#endif
