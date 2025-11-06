#include "internal/instruments/european_option.hpp"
#include "internal/methods/monte_carlo.hpp"
#include "internal/random.hpp"
#include "internal/variance_reduction/control_variates.hpp"
#include "internal/variance_reduction/stratified_sampling.hpp"
#include <cmath>

namespace mcoptions {

double price_european_option(Context& ctx, const OptionData& option) {
    double sum_payoff = 0.0;
    double sum_control = 0.0;  // For control variates
    
    size_t effective_paths = ctx.get_antithetic() ? ctx.get_num_simulations() / 2 : ctx.get_num_simulations();
    
    for (size_t i = 0; i < effective_paths; ++i) {
        // Generate random samples (stratified if enabled)
        std::vector<double> normals;
        if (ctx.get_stratified_sampling()) {
            normals = generate_stratified_normals(ctx.get_rng(), ctx.get_num_steps());
        } else {
            normals = generate_normal_samples(ctx.get_rng(), ctx.get_num_steps());
        }
        
        // Simulate path
        auto path = simulate_gbm_path(ctx, option.spot, option.rate, option.volatility,
                                       option.time_to_maturity, ctx.get_num_steps(), normals);
        
        double final_spot = path.back();
        double poff = payoff(final_spot, option.strike, option.type);
        sum_payoff += poff;
        
        // For control variates: accumulate payoff
        if (ctx.get_control_variates()) {
            sum_control += poff;
        }
        
        // Antithetic variates
        if (ctx.get_antithetic()) {
            for (auto& z : normals) z = -z;
            auto anti_path = simulate_gbm_path(ctx, option.spot, option.rate, option.volatility,
                                                option.time_to_maturity, ctx.get_num_steps(), normals);
            double anti_final_spot = anti_path.back();
            double anti_poff = payoff(anti_final_spot, option.strike, option.type);
            sum_payoff += anti_poff;
            
            if (ctx.get_control_variates()) {
                sum_control += anti_poff;
            }
        }
    }
    
    size_t total_paths = ctx.get_antithetic() ? ctx.get_num_simulations() : effective_paths;
    double avg_payoff = sum_payoff / total_paths;
    double discounted = discount_factor(option.rate, option.time_to_maturity) * avg_payoff;
    
    // Apply control variates if enabled
    if (ctx.get_control_variates()) {
        double mc_control = discount_factor(option.rate, option.time_to_maturity) * (sum_control / total_paths);
        double analytical_control = black_scholes::price(
            option.spot, option.strike, option.rate, 
            option.volatility, option.time_to_maturity, option.type
        );
        
        discounted = apply_control_variate(discounted, mc_control, analytical_control);
    }
    
    return discounted;
}

}
