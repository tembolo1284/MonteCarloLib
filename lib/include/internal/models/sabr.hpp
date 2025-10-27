#ifndef MCOPTIONS_SABR_HPP
#define MCOPTIONS_SABR_HPP

#include "internal/context.hpp"
#include <vector>

namespace mcoptions {

// SABR model path simulation - TODO: Implement
// For now, this is a stub
std::vector<double> simulate_sabr_path(
    const Context& ctx,
    double spot,
    double rate,
    double time_to_maturity,
    size_t num_steps,
    const std::vector<double>& random_normals1,
    const std::vector<double>& random_normals2
);

}

#endif
