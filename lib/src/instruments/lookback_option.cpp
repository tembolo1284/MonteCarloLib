#include "internal/instruments/lookback_option.hpp"
#include "internal/methods/monte_carlo.hpp"
#include "internal/random.hpp"
#include <cmath>
#include <algorithm>

namespace mcoptions {

double price_lookback_option(Context& ctx, const LookbackOptionData& option) {
    double sum_payoff = 0.0;
    
    size_t effective_paths = ctx.antithetic_enabled ? ctx.num_simulations / 2 : ctx.num_simulations;
    
    for (size_t i = 0; i < effective_paths; ++i) {
        auto normals = generate_normal_samples(ctx.rng, ctx.num_steps);
        auto path = simulate_gbm_path(ctx, option.spot, option.rate, option.volatility,
                                       option.time_to_maturity, ctx.num_steps, normals);
        
        // Find max and min along path
        double max_spot = *std::max_element(path.begin(), path.end());
        double min_spot = *std::min_element(path.begin(), path.end());
        double final_spot = path.back();
        
        double poff = 0.0;
        
        if (option.fixed_strike) {
            // Fixed strike lookback
            if (option.type == OptionType::Call) {
                // Payoff = max(S_max - K, 0)
                poff = std::max(0.0, max_spot - option.strike);
            } else {
                // Payoff = max(K - S_min, 0)
                poff = std::max(0.0, option.strike - min_spot);
            }
        } else {
            // Floating strike lookback
            if (option.type == OptionType::Call) {
                // Payoff = S_T - S_min (always positive)
                poff = final_spot - min_spot;
            } else {
                // Payoff = S_max - S_T (always positive)
                poff = max_spot - final_spot;
            }
        }
        
        sum_payoff += poff;
        
        // Antithetic variates
        if (ctx.antithetic_enabled) {
            for (auto& z : normals) z = -z;
            auto anti_path = simulate_gbm_path(ctx, option.spot, option.rate, option.volatility,
                                                option.time_to_maturity, ctx.num_steps, normals);
            
            double anti_max_spot = *std::max_element(anti_path.begin(), anti_path.end());
            double anti_min_spot = *std::min_element(anti_path.begin(), anti_path.end());
            double anti_final_spot = anti_path.back();
            
            double anti_poff = 0.0;
            
            if (option.fixed_strike) {
                if (option.type == OptionType::Call) {
                    anti_poff = std::max(0.0, anti_max_spot - option.strike);
                } else {
                    anti_poff = std::max(0.0, option.strike - anti_min_spot);
                }
            } else {
                if (option.type == OptionType::Call) {
                    anti_poff = anti_final_spot - anti_min_spot;
                } else {
                    anti_poff = anti_max_spot - anti_final_spot;
                }
            }
            
            sum_payoff += anti_poff;
        }
    }
    
    size_t total_paths = ctx.antithetic_enabled ? ctx.num_simulations : effective_paths;
    double avg_payoff = sum_payoff / total_paths;
    return discount_factor(option.rate, option.time_to_maturity) * avg_payoff;
}

}
