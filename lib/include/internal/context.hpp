#ifndef MCOPTIONS_CONTEXT_HPP
#define MCOPTIONS_CONTEXT_HPP

#include <random>
#include <cstddef>

namespace mcoptions {

class Context {
public:
    Context();
    ~Context() = default;

    // Monte Carlo settings
    void set_num_simulations(size_t n);
    size_t get_num_simulations() const;
    
    void set_num_steps(size_t n);
    size_t get_num_steps() const;
    
    void set_antithetic(bool enabled);
    bool get_antithetic() const;
    
    void set_control_variates(bool enabled);
    bool get_control_variates() const;
    
    // Binomial tree settings (NEW)
    void set_binomial_steps(size_t n);
    size_t get_binomial_steps() const;
    
    // Random number generation
    std::mt19937& get_rng();
    void set_seed(unsigned int seed);

private:
    // Monte Carlo configuration
    size_t num_simulations_;
    size_t num_steps_;
    bool antithetic_enabled_;
    bool control_variates_enabled_;
    
    // Binomial tree configuration (NEW)
    size_t binomial_steps_;  // Default: 100
    
    // Random number generator
    std::mt19937 rng_;
};

}

#endif
