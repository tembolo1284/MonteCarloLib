#include "internal/instruments/barrier_option.hpp"
#include "internal/methods/monte_carlo.hpp"
#include "internal/random.hpp"
#include <cmath>
#include <algorithm>

namespace mcoptions {

double price_barrier_option(Context& ctx, const BarrierOptionData& option) {
    double sum_payoff = 0.0;
    
    size_t effective_paths = ctx.get_antithetic() ? ctx.get_num_simulations() / 2 : ctx.get_num_simulations();
    
    for (size_t i = 0; i < effective_paths; ++i) {
        auto normals = generate_normal_samples(ctx.get_rng(), ctx.get_num_steps());
        auto path = simulate_gbm_path(ctx, option.spot, option.rate, option.volatility,
                                       option.time_to_maturity, ctx.get_num_steps(), normals);
        
        // Check if barrier was hit
        bool barrier_hit = false;
        
        if (option.barrier_type == BarrierType::UpAndOut || option.barrier_type == BarrierType::UpAndIn) {
            // Check if any spot >= barrier
            for (double spot : path) {
                if (spot >= option.barrier_level) {
                    barrier_hit = true;
                    break;
                }
            }
        } else { // DownAndOut or DownAndIn
            // Check if any spot <= barrier
            for (double spot : path) {
                if (spot <= option.barrier_level) {
                    barrier_hit = true;
                    break;
                }
            }
        }
        
        double poff = 0.0;
        
        if (option.barrier_type == BarrierType::UpAndOut || option.barrier_type == BarrierType::DownAndOut) {
            // Knock-out: pay only if barrier NOT hit
            if (!barrier_hit) {
                double final_spot = path.back();
                poff = payoff(final_spot, option.strike, option.type);
            } else {
                poff = option.rebate;  // Pay rebate if knocked out
            }
        } else { // UpAndIn or DownAndIn
            // Knock-in: pay only if barrier WAS hit
            if (barrier_hit) {
                double final_spot = path.back();
                poff = payoff(final_spot, option.strike, option.type);
            } else {
                poff = option.rebate;  // Pay rebate if not knocked in
            }
        }
        
        sum_payoff += poff;
        
        // Antithetic variates
        if (ctx.get_antithetic()) {
            for (auto& z : normals) z = -z;
            auto anti_path = simulate_gbm_path(ctx, option.spot, option.rate, option.volatility,
                                                option.time_to_maturity, ctx.get_num_steps(), normals);
            
            bool anti_barrier_hit = false;
            
            if (option.barrier_type == BarrierType::UpAndOut || option.barrier_type == BarrierType::UpAndIn) {
                for (double spot : anti_path) {
                    if (spot >= option.barrier_level) {
                        anti_barrier_hit = true;
                        break;
                    }
                }
            } else {
                for (double spot : anti_path) {
                    if (spot <= option.barrier_level) {
                        anti_barrier_hit = true;
                        break;
                    }
                }
            }
            
            double anti_poff = 0.0;
            
            if (option.barrier_type == BarrierType::UpAndOut || option.barrier_type == BarrierType::DownAndOut) {
                if (!anti_barrier_hit) {
                    double anti_final_spot = anti_path.back();
                    anti_poff = payoff(anti_final_spot, option.strike, option.type);
                } else {
                    anti_poff = option.rebate;
                }
            } else {
                if (anti_barrier_hit) {
                    double anti_final_spot = anti_path.back();
                    anti_poff = payoff(anti_final_spot, option.strike, option.type);
                } else {
                    anti_poff = option.rebate;
                }
            }
            
            sum_payoff += anti_poff;
        }
    }
    
    size_t total_paths = ctx.get_antithetic() ? ctx.get_num_simulations() : effective_paths;
    double avg_payoff = sum_payoff / total_paths;
    return discount_factor(option.rate, option.time_to_maturity) * avg_payoff;
}

}
