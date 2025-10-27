#include "internal/european_option.hpp"
#include "internal/monte_carlo.hpp"
#include "internal/random.hpp"
#include <cmath>

namespace mcoptions {

double price_european_option(Context& ctx, const OptionData& option) {
    double sum_payoff = 0.0;
    double sum_payoff_sq = 0.0;
    
    size_t effective_paths = ctx.antithetic_enabled ? ctx.num_simulations / 2 : ctx.num_simulations;
    
    for (size_t i = 0; i < effective_paths; ++i) {
        auto normals = generate_normal_samples(ctx.rng, ctx.num_steps);
        auto path = simulate_gbm_path(ctx, option.spot, option.rate, option.volatility,
                                       option.time_to_maturity, ctx.num_steps, normals);
        
        double final_spot = path.back();
        double poff = payoff(final_spot, option.strike, option.type);
        sum_payoff += poff;
        
        if (ctx.antithetic_enabled) {
            for (auto& z : normals) z = -z;
            auto anti_path = simulate_gbm_path(ctx, option.spot, option.rate, option.volatility,
                                                option.time_to_maturity, ctx.num_steps, normals);
            double anti_final_spot = anti_path.back();
            double anti_poff = payoff(anti_final_spot, option.strike, option.type);
            sum_payoff += anti_poff;
        }
    }
    
    size_t total_paths = ctx.antithetic_enabled ? ctx.num_simulations : effective_paths;
    double avg_payoff = sum_payoff / total_paths;
    return discount_factor(option.rate, option.time_to_maturity) * avg_payoff;
}

}
