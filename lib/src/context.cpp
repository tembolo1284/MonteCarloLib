#include "internal/context.hpp"

namespace mcoptions {

Context::Context()
    : seed(12345)
    , num_simulations(100000)
    , num_steps(252)
    , antithetic_enabled(true)
    , importance_sampling_enabled(false)
    , drift_shift(0.0)
    , rng(seed) { }

void Context::reset_rng() {
    rng.seed(seed);
}

}
