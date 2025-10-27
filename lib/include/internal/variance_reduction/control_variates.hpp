#ifndef MCOPTIONS_CONTROL_VARIATES_HPP
#define MCOPTIONS_CONTROL_VARIATES_HPP

#include "internal/instruments/instrument.hpp"
#include <cmath>

namespace mcoptions {

// Black-Scholes analytical formulas (used as control)
namespace black_scholes {

inline double normal_cdf(double x) {
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

inline double d1(double spot, double strike, double rate, double volatility, double time) {
    return (std::log(spot / strike) + (rate + 0.5 * volatility * volatility) * time) 
           / (volatility * std::sqrt(time));
}

inline double d2(double spot, double strike, double rate, double volatility, double time) {
    return d1(spot, strike, rate, volatility, time) - volatility * std::sqrt(time);
}

inline double call_price(double spot, double strike, double rate, double volatility, double time) {
    if (time <= 0.0) return std::max(0.0, spot - strike);
    double d_1 = d1(spot, strike, rate, volatility, time);
    double d_2 = d2(spot, strike, rate, volatility, time);
    return spot * normal_cdf(d_1) - strike * std::exp(-rate * time) * normal_cdf(d_2);
}

inline double put_price(double spot, double strike, double rate, double volatility, double time) {
    if (time <= 0.0) return std::max(0.0, strike - spot);
    double d_1 = d1(spot, strike, rate, volatility, time);
    double d_2 = d2(spot, strike, rate, volatility, time);
    return strike * std::exp(-rate * time) * normal_cdf(-d_2) - spot * normal_cdf(-d_1);
}

inline double price(double spot, double strike, double rate, double volatility, 
                   double time, OptionType type) {
    return type == OptionType::Call 
        ? call_price(spot, strike, rate, volatility, time)
        : put_price(spot, strike, rate, volatility, time);
}

} // namespace black_scholes

// Apply control variate correction to Monte Carlo estimate
inline double apply_control_variate(double mc_estimate, double mc_control, 
                                    double analytical_control) {
    // Optimal beta = 1.0 when using same random numbers
    return mc_estimate - (mc_control - analytical_control);
}

} // namespace mcoptions

#endif
