#ifndef MCOPTIONS_MONTE_CARLO_HPP
#define MCOPTIONS_MONTE_CARLO_HPP

#include "context.hpp"
#include "instrument.hpp"
#include <vector>

namespace mcoptions {

std::vector<double> simulate_gbm_path(
    const Context& ctx,
    double spot,
    double rate,
    double volatility,
    double time_to_maturity,
    size_t num_steps,
    const std::vector<double>& random_normals
);

double discount_factor(double rate, double time);

}

#endif
