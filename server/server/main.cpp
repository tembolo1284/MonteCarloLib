#include "mcoptions_service.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <csignal>
#include <atomic>

// Global server pointer for signal handler
std::unique_ptr<grpc::Server> g_server;
std::atomic<bool> g_shutdown_requested(false);

void SignalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        std::cout << std::endl << std::endl;
        std::cout << COLOR_YELLOW << "============================================" << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "  Graceful shutdown initiated..." << COLOR_RESET << std::endl;
        std::cout << COLOR_YELLOW << "============================================" << COLOR_RESET << std::endl;
        
        if (g_server && !g_shutdown_requested.exchange(true)) {
            // Set a deadline for shutdown (5 seconds)
            auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(5);
            g_server->Shutdown(deadline);
            
            std::cout << "  Waiting for active requests to complete..." << std::endl;
            std::cout << "  (max 5 seconds)" << std::endl;
        }
    }
}

void RunServer(const std::string& server_address) {
    McOptionsServiceImpl service;
    
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    
    g_server = builder.BuildAndStart();
    
    // Register signal handlers
    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);
    
    std::cout << COLOR_GREEN << "============================================" << COLOR_RESET << std::endl;
    std::cout << COLOR_GREEN << "  Monte Carlo Options Pricing Server" << COLOR_RESET << std::endl;
    std::cout << COLOR_GREEN << "============================================" << COLOR_RESET << std::endl;
    std::cout << "Server listening on " << COLOR_CYAN << server_address << COLOR_RESET << std::endl;
    std::cout << std::endl;
    std::cout << "Available endpoints:" << std::endl;
    std::cout << "  - PriceEuropeanCall/Put" << std::endl;
    std::cout << "  - PriceAmericanCall/Put" << std::endl;
    std::cout << "  - PriceAsianCall/Put" << std::endl;
    std::cout << "  - PriceBarrierCall/Put" << std::endl;
    std::cout << "  - PriceLookbackCall/Put" << std::endl;
    std::cout << "  - PriceBermudanCall/Put" << std::endl;
    std::cout << "  - PriceBatch" << std::endl;
    std::cout << std::endl;
    
    g_server->Wait();
    
    std::cout << std::endl;
    std::cout << COLOR_GREEN << "✓ Server shutdown complete" << COLOR_RESET << std::endl;
    std::cout << COLOR_GREEN << "============================================" << COLOR_RESET << std::endl;
}

int main(int argc, char** argv) {
    std::string server_address = "0.0.0.0:50051";
    
    if (argc > 1) {
        server_address = argv[1];
    }
    
    RunServer(server_address);
    
    return 0;
}
