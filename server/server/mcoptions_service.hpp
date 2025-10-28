#ifndef MCOPTIONS_SERVICE_HPP
#define MCOPTIONS_SERVICE_HPP

#include <grpcpp/grpcpp.h>
#include "mcoptions.grpc.pb.h"
#include "mcoptions.h"
#include "logging.hpp"
#include "request_handlers.hpp"
#include <chrono>
#include <memory>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

class McOptionsServiceImpl final : public mcoptions::McOptionsService::Service {
public:
    Status PriceEuropeanCall(ServerContext* context,
                            const mcoptions::EuropeanRequest* request,
                            mcoptions::PriceResponse* response) override {
        mcoptions::logging::log_request("PriceEuropeanCall", 
            mcoptions::handlers::format_european_params(request));
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ctx = mco_context_new();
        mcoptions::handlers::apply_config(ctx, request->config());
        
        double price = mco_european_call(ctx, request->spot(), request->strike(),
            request->rate(), request->volatility(), request->time_to_maturity());
        
        mco_context_free(ctx);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        response->set_price(price);
        response->set_computation_time_ms(duration.count());
        mcoptions::logging::log_result(price, duration.count());
        
        return Status::OK;
    }
    
    Status PriceEuropeanPut(ServerContext* context,
                           const mcoptions::EuropeanRequest* request,
                           mcoptions::PriceResponse* response) override {
        mcoptions::logging::log_request("PriceEuropeanPut", 
            mcoptions::handlers::format_european_params(request));
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ctx = mco_context_new();
        mcoptions::handlers::apply_config(ctx, request->config());
        
        double price = mco_european_put(ctx, request->spot(), request->strike(),
            request->rate(), request->volatility(), request->time_to_maturity());
        
        mco_context_free(ctx);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        response->set_price(price);
        response->set_computation_time_ms(duration.count());
        mcoptions::logging::log_result(price, duration.count());
        
        return Status::OK;
    }
    
    Status PriceAmericanCall(ServerContext* context,
                            const mcoptions::AmericanRequest* request,
                            mcoptions::PriceResponse* response) override {
        mcoptions::logging::log_request("PriceAmericanCall", 
            mcoptions::handlers::format_american_params(request));
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ctx = mco_context_new();
        mcoptions::handlers::apply_config(ctx, request->config());
        
        double price = mco_american_call(ctx, request->spot(), request->strike(),
            request->rate(), request->volatility(), request->time_to_maturity(),
            request->num_exercise_points());
        
        mco_context_free(ctx);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        response->set_price(price);
        response->set_computation_time_ms(duration.count());
        mcoptions::logging::log_result(price, duration.count());
        
        return Status::OK;
    }
    
    Status PriceAmericanPut(ServerContext* context,
                           const mcoptions::AmericanRequest* request,
                           mcoptions::PriceResponse* response) override {
        mcoptions::logging::log_request("PriceAmericanPut", 
            mcoptions::handlers::format_american_params(request));
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ctx = mco_context_new();
        mcoptions::handlers::apply_config(ctx, request->config());
        
        double price = mco_american_put(ctx, request->spot(), request->strike(),
            request->rate(), request->volatility(), request->time_to_maturity(),
            request->num_exercise_points());
        
        mco_context_free(ctx);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        response->set_price(price);
        response->set_computation_time_ms(duration.count());
        mcoptions::logging::log_result(price, duration.count());
        
        return Status::OK;
    }
    
    Status PriceAsianCall(ServerContext* context,
                         const mcoptions::AsianRequest* request,
                         mcoptions::PriceResponse* response) override {
        mcoptions::logging::log_request("PriceAsianCall", 
            mcoptions::handlers::format_asian_params(request));
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ctx = mco_context_new();
        mcoptions::handlers::apply_config(ctx, request->config());
        
        double price = mco_asian_arithmetic_call(ctx, request->spot(), request->strike(),
            request->rate(), request->volatility(), request->time_to_maturity(),
            request->num_observations());
        
        mco_context_free(ctx);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        response->set_price(price);
        response->set_computation_time_ms(duration.count());
        mcoptions::logging::log_result(price, duration.count());
        
        return Status::OK;
    }
    
    Status PriceAsianPut(ServerContext* context,
                        const mcoptions::AsianRequest* request,
                        mcoptions::PriceResponse* response) override {
        mcoptions::logging::log_request("PriceAsianPut", 
            mcoptions::handlers::format_asian_params(request));
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ctx = mco_context_new();
        mcoptions::handlers::apply_config(ctx, request->config());
        
        double price = mco_asian_arithmetic_put(ctx, request->spot(), request->strike(),
            request->rate(), request->volatility(), request->time_to_maturity(),
            request->num_observations());
        
        mco_context_free(ctx);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        response->set_price(price);
        response->set_computation_time_ms(duration.count());
        mcoptions::logging::log_result(price, duration.count());
        
        return Status::OK;
    }
    
    Status PriceBarrierCall(ServerContext* context,
                           const mcoptions::BarrierRequest* request,
                           mcoptions::PriceResponse* response) override {
        mcoptions::logging::log_request("PriceBarrierCall", 
            mcoptions::handlers::format_barrier_params(request));
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ctx = mco_context_new();
        mcoptions::handlers::apply_config(ctx, request->config());
        
        double price = mco_barrier_call(ctx, request->spot(), request->strike(),
            request->rate(), request->volatility(), request->time_to_maturity(),
            request->barrier_level(), static_cast<int>(request->barrier_type()),
            request->rebate());
        
        mco_context_free(ctx);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        response->set_price(price);
        response->set_computation_time_ms(duration.count());
        mcoptions::logging::log_result(price, duration.count());
        
        return Status::OK;
    }
    
    Status PriceBarrierPut(ServerContext* context,
                          const mcoptions::BarrierRequest* request,
                          mcoptions::PriceResponse* response) override {
        mcoptions::logging::log_request("PriceBarrierPut", 
            mcoptions::handlers::format_barrier_params(request));
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ctx = mco_context_new();
        mcoptions::handlers::apply_config(ctx, request->config());
        
        double price = mco_barrier_put(ctx, request->spot(), request->strike(),
            request->rate(), request->volatility(), request->time_to_maturity(),
            request->barrier_level(), static_cast<int>(request->barrier_type()),
            request->rebate());
        
        mco_context_free(ctx);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        response->set_price(price);
        response->set_computation_time_ms(duration.count());
        mcoptions::logging::log_result(price, duration.count());
        
        return Status::OK;
    }
    
    Status PriceLookbackCall(ServerContext* context,
                            const mcoptions::LookbackRequest* request,
                            mcoptions::PriceResponse* response) override {
        mcoptions::logging::log_request("PriceLookbackCall", 
            mcoptions::handlers::format_lookback_params(request));
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ctx = mco_context_new();
        mcoptions::handlers::apply_config(ctx, request->config());
        
        double price = mco_lookback_call(ctx, request->spot(), request->strike(),
            request->rate(), request->volatility(), request->time_to_maturity(),
            request->fixed_strike());
        
        mco_context_free(ctx);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        response->set_price(price);
        response->set_computation_time_ms(duration.count());
        mcoptions::logging::log_result(price, duration.count());
        
        return Status::OK;
    }
    
    Status PriceLookbackPut(ServerContext* context,
                           const mcoptions::LookbackRequest* request,
                           mcoptions::PriceResponse* response) override {
        mcoptions::logging::log_request("PriceLookbackPut", 
            mcoptions::handlers::format_lookback_params(request));
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ctx = mco_context_new();
        mcoptions::handlers::apply_config(ctx, request->config());
        
        double price = mco_lookback_put(ctx, request->spot(), request->strike(),
            request->rate(), request->volatility(), request->time_to_maturity(),
            request->fixed_strike());
        
        mco_context_free(ctx);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        response->set_price(price);
        response->set_computation_time_ms(duration.count());
        mcoptions::logging::log_result(price, duration.count());
        
        return Status::OK;
    }
    
    Status PriceBermudanCall(ServerContext* context,
                            const mcoptions::BermudanRequest* request,
                            mcoptions::PriceResponse* response) override {
        mcoptions::logging::log_request("PriceBermudanCall", 
            mcoptions::handlers::format_bermudan_params(request));
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ctx = mco_context_new();
        mcoptions::handlers::apply_config(ctx, request->config());
        
        std::vector<double> dates(request->exercise_dates().begin(), 
                                 request->exercise_dates().end());
        double price = mco_bermudan_call(ctx, request->spot(), request->strike(),
            request->rate(), request->volatility(), dates.data(), dates.size());
        
        mco_context_free(ctx);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        response->set_price(price);
        response->set_computation_time_ms(duration.count());
        mcoptions::logging::log_result(price, duration.count());
        
        return Status::OK;
    }
    
    Status PriceBermudanPut(ServerContext* context,
                           const mcoptions::BermudanRequest* request,
                           mcoptions::PriceResponse* response) override {
        mcoptions::logging::log_request("PriceBermudanPut", 
            mcoptions::handlers::format_bermudan_params(request));
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ctx = mco_context_new();
        mcoptions::handlers::apply_config(ctx, request->config());
        
        std::vector<double> dates(request->exercise_dates().begin(), 
                                 request->exercise_dates().end());
        double price = mco_bermudan_put(ctx, request->spot(), request->strike(),
            request->rate(), request->volatility(), dates.data(), dates.size());
        
        mco_context_free(ctx);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        response->set_price(price);
        response->set_computation_time_ms(duration.count());
        mcoptions::logging::log_result(price, duration.count());
        
        return Status::OK;
    }
    
    Status PriceBatch(ServerContext* context,
                     const mcoptions::BatchRequest* request,
                     mcoptions::BatchResponse* response) override {
        mcoptions::logging::log_request("PriceBatch", 
            "Calls=" + std::to_string(request->european_calls_size()) + 
            ", Puts=" + std::to_string(request->european_puts_size()));
        
        auto start = std::chrono::high_resolution_clock::now();
        auto ctx = mco_context_new();
        mcoptions::handlers::apply_config(ctx, request->config());
        
        for (const auto& req : request->european_calls()) {
            double price = mco_european_call(ctx, req.spot(), req.strike(),
                req.rate(), req.volatility(), req.time_to_maturity());
            response->add_european_call_prices(price);
        }
        
        for (const auto& req : request->european_puts()) {
            double price = mco_european_put(ctx, req.spot(), req.strike(),
                req.rate(), req.volatility(), req.time_to_maturity());
            response->add_european_put_prices(price);
        }
        
        mco_context_free(ctx);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        response->set_total_computation_time_ms(duration.count());
        mcoptions::logging::log_batch_complete(duration.count());
        
        return Status::OK;
    }
};

#endif
