#ifndef MCOPTIONS_REQUEST_HANDLERS_HPP
#define MCOPTIONS_REQUEST_HANDLERS_HPP

#include "mcoptions.h"
#include "mcoptions.grpc.pb.h"
#include "logging.hpp"
#include <chrono>
#include <sstream>

namespace mcoptions {
namespace handlers {

inline void apply_config(mco_context_t* ctx, const SimulationConfig& config) {
    if (config.num_simulations() > 0) {
        mco_context_set_num_simulations(ctx, config.num_simulations());
    }
    if (config.num_steps() > 0) {
        mco_context_set_num_steps(ctx, config.num_steps());
    }
    if (config.seed() > 0) {
        mco_context_set_seed(ctx, config.seed());
    }
    mco_context_set_antithetic(ctx, config.antithetic_enabled());
    mco_context_set_control_variates(ctx, config.control_variates_enabled());
    mco_context_set_stratified_sampling(ctx, config.stratified_sampling_enabled());
}

inline std::string format_european_params(const EuropeanRequest* request) {
    std::stringstream ss;
    ss << "S=" << request->spot() << ", K=" << request->strike() 
       << ", r=" << request->rate() << ", σ=" << request->volatility() 
       << ", T=" << request->time_to_maturity() << " | " 
       << logging::format_config(request->config());
    return ss.str();
}

inline std::string format_american_params(const AmericanRequest* request) {
    std::stringstream ss;
    ss << "S=" << request->spot() << ", K=" << request->strike() 
       << ", r=" << request->rate() << ", σ=" << request->volatility() 
       << ", T=" << request->time_to_maturity() 
       << ", ExPoints=" << request->num_exercise_points() << " | " 
       << logging::format_config(request->config());
    return ss.str();
}

inline std::string format_asian_params(const AsianRequest* request) {
    std::stringstream ss;
    ss << "S=" << request->spot() << ", K=" << request->strike() 
       << ", r=" << request->rate() << ", σ=" << request->volatility() 
       << ", T=" << request->time_to_maturity() 
       << ", Obs=" << request->num_observations() << " | " 
       << logging::format_config(request->config());
    return ss.str();
}

inline std::string format_barrier_params(const BarrierRequest* request) {
    std::stringstream ss;
    ss << "S=" << request->spot() << ", K=" << request->strike() 
       << ", r=" << request->rate() << ", σ=" << request->volatility() 
       << ", T=" << request->time_to_maturity() 
       << ", Barrier=" << request->barrier_level() 
       << ", Type=" << request->barrier_type() << " | " 
       << logging::format_config(request->config());
    return ss.str();
}

inline std::string format_lookback_params(const LookbackRequest* request) {
    std::stringstream ss;
    ss << "S=" << request->spot() << ", K=" << request->strike() 
       << ", r=" << request->rate() << ", σ=" << request->volatility() 
       << ", T=" << request->time_to_maturity() 
       << ", Fixed=" << (request->fixed_strike() ? "Yes" : "No") << " | " 
       << logging::format_config(request->config());
    return ss.str();
}

inline std::string format_bermudan_params(const BermudanRequest* request) {
    std::stringstream ss;
    ss << "S=" << request->spot() << ", K=" << request->strike() 
       << ", r=" << request->rate() << ", σ=" << request->volatility() 
       << ", ExDates=" << request->exercise_dates_size() << " | " 
       << logging::format_config(request->config());
    return ss.str();
}

} // namespace handlers
} // namespace mcoptions

#endif
