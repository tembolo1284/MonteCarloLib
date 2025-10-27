#include "internal/instruments/asian_option.hpp"
#include "internal/methods/monte_carlo.hpp"
#include "internal/random.hpp"
#include <cmath>
#include <numeric>

namespace mcoptions {

double price_asian_option(Context& ctx, const AsianOptionData& option) {
    double sum_payoff = 0.0;
    
    size_t effective_paths = ctx.antithetic_enabled ? ctx.num_simulations / 2 : ctx.num_simulations;
    
    for (size_t i = 0; i < effective_paths; ++i) {
        auto normals = generate_normal_samples(ctx.rng, ctx.num_steps);
        auto path = simulate_gbm_path(ctx, option.spot, option.rate, option.volatility,
                                       option.time_to_maturity, ctx.num_steps, normals);
        
        size_t obs_step = ctx.num_steps / option.num_observations;
        double sum_spots = 0.0;
        for (int j = 0; j < option.num_observations; ++j) {
            size_t idx = std::min((j + 1) * obs_step, ctx.num_steps);
            sum_spots += path[idx];
        }
        double avg_spot = sum_spots / option.num_observations;
        double poff = payoff(avg_spot, option.strike, option.type);
        sum_payoff += poff;
        
        if (ctx.antithetic_enabled) {
            for (auto& z : normals) z = -z;
            auto anti_path = simulate_gbm_path(ctx, option.spot, option.rate, option.volatility,
                                                option.time_to_maturity, ctx.num_steps, normals);
            double anti_sum_spots = 0.0;
            for (int j = 0; j < option.num_observations; ++j) {
                size_t idx = std::min((j + 1) * obs_step, ctx.num_steps);
                anti_sum_spots += anti_path[idx];
            }
            double anti_avg_spot = anti_sum_spots / option.num_observations;
            double anti_poff = payoff(anti_avg_spot, option.strike, option.type);
            sum_payoff += anti_poff;
        }
    }
    
    size_t total_paths = ctx.antithetic_enabled ? ctx.num_simulations : effective_paths;
    double avg_payoff = sum_payoff / total_paths;
    return discount_factor(option.rate, option.time_to_maturity) * avg_payoff;
}

}
