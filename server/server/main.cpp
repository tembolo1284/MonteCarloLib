#include "mcoptions_service.hpp"
#include <iostream>
#include <memory>
#include <string>

void RunServer(const std::string& server_address) {
    McOptionsServiceImpl service;
    
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    
    std::cout << "============================================" << std::endl;
    std::cout << "  Monte Carlo Options Pricing Server" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "Server listening on " << server_address << std::endl;
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
    std::cout << "Press Ctrl+C to stop" << std::endl;
    std::cout << "============================================" << std::endl;
    
    server->Wait();
}

int main(int argc, char** argv) {
    std::string server_address = "0.0.0.0:50051";
    
    if (argc > 1) {
        server_address = argv[1];
    }
    
    RunServer(server_address);
    
    return 0;
}
