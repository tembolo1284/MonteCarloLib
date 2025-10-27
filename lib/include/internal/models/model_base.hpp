#ifndef MCOPTIONS_MODEL_BASE_HPP
#define MCOPTIONS_MODEL_BASE_HPP

#include "internal/context.hpp"
#include <vector>

namespace mcoptions {

// Base interface for all models (future use)
class Model {
public:
    virtual ~Model() = default;
    virtual std::vector<double> simulate_path(
        const Context& ctx,
        double spot,
        double rate,
        double volatility,
        double time_to_maturity,
        size_t num_steps,
        const std::vector<double>& random_normals
    ) = 0;
};

}

#endif
