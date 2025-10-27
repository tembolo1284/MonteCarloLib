#ifndef MCOPTIONS_GBM_HPP
#define MCOPTIONS_GBM_HPP

#include "internal/context.hpp"
#include <vector>

namespace mcoptions {

// Geometric Brownian Motion path simulation
// This is what we currently use in simulate_gbm_path()
std::vector<double> simulate_gbm_path(
    const Context& ctx,
    double spot,
    double rate,
    double volatility,
    double time_to_maturity,
    size_t num_steps,
    const std::vector<double>& random_normals
);

}

#endif
