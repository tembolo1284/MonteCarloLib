#ifndef MCOPTIONS_CONTEXT_HPP
#define MCOPTIONS_CONTEXT_HPP

#include <cstdint>
#include <random>

namespace mcoptions {

struct Context {
    // RNG settings
    uint64_t seed;
    uint64_t num_simulations;
    uint64_t num_steps;
    std::mt19937_64 rng;
    
    // Variance Reduction techniques
    bool antithetic_enabled;              // Existing
    bool importance_sampling_enabled;     // Existing
    double drift_shift;                   // Existing (for importance sampling)
    bool control_variates_enabled;        // NEW
    bool stratified_sampling_enabled;     // NEW
    
    // Model selection
    enum class Model {
        GBM,     // Geometric Brownian Motion (current)
        SABR     // Stochastic Alpha Beta Rho (NEW)
    };
    Model model;
    
    // SABR parameters (only used if model == SABR)
    double sabr_alpha;  // Initial volatility
    double sabr_beta;   // CEV exponent (0=normal, 0.5=CIR, 1=lognormal)
    double sabr_rho;    // Correlation between forward and vol
    double sabr_nu;     // Volatility of volatility
    
    Context();
    void reset_rng();
};

}

#endif
