#include <grpcpp/grpcpp.h>
#include "mcoptions.grpc.pb.h"
#include <iostream>
#include <memory>
#include <iomanip>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class McOptionsClient {
public:
    McOptionsClient(std::shared_ptr<Channel> channel)
        : stub_(mcoptions::McOptionsService::NewStub(channel)) {}
    
    void PriceEuropeanCall(double spot, double strike, double rate,
                          double volatility, double time_to_maturity) {
        mcoptions::EuropeanRequest request;
        request.set_spot(spot);
        request.set_strike(strike);
        request.set_rate(rate);
        request.set_volatility(volatility);
        request.set_time_to_maturity(time_to_maturity);
        
        auto* config = request.mutable_config();
        config->set_num_simulations(100000);
        config->set_num_steps(252);
        config->set_antithetic_enabled(true);
        config->set_control_variates_enabled(true);
        
        mcoptions::PriceResponse response;
        ClientContext context;
        
        Status status = stub_->PriceEuropeanCall(&context, request, &response);
        
        if (status.ok()) {
            std::cout << "  European Call: $" << std::fixed << std::setprecision(4)
                     << response.price() << " (" << response.computation_time_ms() << "ms)" 
                     << std::endl;
        } else {
            std::cout << "  RPC failed: " << status.error_message() << std::endl;
        }
    }
    
    void PriceAmericanPut(double spot, double strike, double rate,
                         double volatility, double time_to_maturity) {
        mcoptions::AmericanRequest request;
        request.set_spot(spot);
        request.set_strike(strike);
        request.set_rate(rate);
        request.set_volatility(volatility);
        request.set_time_to_maturity(time_to_maturity);
        request.set_num_exercise_points(50);
        
        auto* config = request.mutable_config();
        config->set_num_simulations(50000);
        config->set_num_steps(252);
        
        mcoptions::PriceResponse response;
        ClientContext context;
        
        Status status = stub_->PriceAmericanPut(&context, request, &response);
        
        if (status.ok()) {
            std::cout << "  American Put:  $" << std::fixed << std::setprecision(4)
                     << response.price() << " (" << response.computation_time_ms() << "ms)" 
                     << std::endl;
        } else {
            std::cout << "  RPC failed: " << status.error_message() << std::endl;
        }
    }
    
private:
    std::unique_ptr<mcoptions::McOptionsService::Stub> stub_;
};

int main(int argc, char** argv) {
    std::string server_address = "localhost:50051";
    
    if (argc > 1) {
        server_address = argv[1];
    }
    
    McOptionsClient client(
        grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));
    
    std::cout << "============================================" << std::endl;
    std::cout << "  Monte Carlo Options Pricing Client" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "Connecting to: " << server_address << std::endl;
    std::cout << std::endl;
    
    // Example 1: ATM Options
    std::cout << "Pricing ATM options (S=100, K=100, r=5%, σ=20%, T=1y):" << std::endl;
    client.PriceEuropeanCall(100.0, 100.0, 0.05, 0.2, 1.0);
    client.PriceAmericanPut(100.0, 100.0, 0.05, 0.2, 1.0);
    
    std::cout << std::endl;
    
    // Example 2: ITM Options
    std::cout << "Pricing ITM options (S=120, K=100, r=5%, σ=20%, T=1y):" << std::endl;
    client.PriceEuropeanCall(120.0, 100.0, 0.05, 0.2, 1.0);
    
    std::cout << std::endl;
    std::cout << "============================================" << std::endl;
    
    return 0;
}
