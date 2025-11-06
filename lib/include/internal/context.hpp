#ifndef MCOPTIONS_CONTEXT_HPP
#define MCOPTIONS_CONTEXT_HPP

#include <random>
#include <cstddef>

namespace mcoptions {

class Context {
public:
    enum class Model {
        BlackScholes,
        Heston,
        SABR
    };

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
    
    void set_stratified_sampling(bool enabled);
    bool get_stratified_sampling() const;
    
    void set_importance_sampling(bool enabled, double drift_shift);
    bool get_importance_sampling() const;
    double get_drift_shift() const;
    
    // Model settings
    void set_model(Model model);
    Model get_model() const;
    
    void set_sabr_params(double alpha, double beta, double rho, double nu);
    double get_sabr_alpha() const;
    double get_sabr_beta() const;
    double get_sabr_rho() const;
    double get_sabr_nu() const;
    
    // Binomial tree settings
    void set_binomial_steps(size_t n);
    size_t get_binomial_steps() const;
    
    // Random number generation
    std::mt19937_64& get_rng();
    void set_seed(unsigned int seed);

private:
    // Monte Carlo configuration
    size_t num_simulations_;
    size_t num_steps_;
    bool antithetic_enabled_;
    bool control_variates_enabled_;
    bool stratified_sampling_enabled_;
    bool importance_sampling_enabled_;
    double drift_shift_;
    
    // Model configuration
    Model model_;
    double sabr_alpha_;
    double sabr_beta_;
    double sabr_rho_;
    double sabr_nu_;
    
    // Binomial tree configuration
    size_t binomial_steps_;
    
    // Random number generator
    std::mt19937_64 rng_;
};

}

#endif
