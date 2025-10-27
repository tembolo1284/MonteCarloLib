#ifndef MCOPTIONS_CONTEXT_HPP
#define MCOPTIONS_CONTEXT_HPP

#include <cstdint>
#include <random>

namespace mcoptions {

struct Context {
    uint64_t seed;
    uint64_t num_simulations;
    uint64_t num_steps;
    bool antithetic_enabled;
    bool importance_sampling_enabled;
    double drift_shift;
    std::mt19937_64 rng;
    
    Context();
    void reset_rng();
};

}

#endif
