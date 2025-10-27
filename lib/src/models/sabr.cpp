#include "internal/models/sabr.hpp"
#include <stdexcept>

namespace mcoptions {

std::vector<double> simulate_sabr_path(
    const Context& ctx,
    double spot,
    double rate,
    double time_to_maturity,
    size_t num_steps,
    const std::vector<double>& random_normals1,
    const std::vector<double>& random_normals2
) {
    // TODO: Implement SABR simulation
    // dF_t = alpha_t * F_t^beta * dW_1
    // dalpha_t = nu * alpha_t * dW_2
    // Correlation: rho
    throw std::runtime_error("SABR model not yet implemented");
}

}
