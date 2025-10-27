#include "internal/monte_carlo.hpp"
#include "internal/random.hpp"
#include <cmath>

namespace mcoptions {

std::vector<double> simulate_gbm_path(
    const Context& ctx,
    double spot,
    double rate,
    double volatility,
    double time_to_maturity,
    size_t num_steps,
    const std::vector<double>& random_normals
) {
    std::vector<double> path(num_steps + 1);
    path[0] = spot;
    
    double dt = time_to_maturity / num_steps;
    double drift = (rate - 0.5 * volatility * volatility) * dt;
    double diffusion = volatility * std::sqrt(dt);
    
    for (size_t i = 0; i < num_steps; ++i) {
        double z = random_normals[i];
        path[i + 1] = path[i] * std::exp(drift + diffusion * z);
    }
    
    return path;
}

double discount_factor(double rate, double time) {
    return std::exp(-rate * time);
}

}
