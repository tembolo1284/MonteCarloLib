#include "internal/context.hpp"

namespace mcoptions {

Context::Context()
    : seed(12345)
    , num_simulations(100000)
    , num_steps(252)
    , rng(seed)
    , antithetic_enabled(true)
    , importance_sampling_enabled(false)
    , drift_shift(0.0)
    , control_variates_enabled(false)
    , stratified_sampling_enabled(false)
    , binomial_steps_(100)  // NEW: Default 100 steps for binomial tree
    , model(Model::GBM)
    , sabr_alpha(0.0)
    , sabr_beta(0.0)
    , sabr_rho(0.0)
    , sabr_nu(0.0)
{
}

void Context::reset_rng() {
    rng.seed(seed);
}

// NEW: Binomial tree configuration
void Context::set_binomial_steps(size_t n) {
    binomial_steps_ = n;
}

size_t Context::get_binomial_steps() const {
    return binomial_steps_;
}

}
