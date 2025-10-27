#include "internal/american_option.hpp"
#include "internal/monte_carlo.hpp"
#include "internal/random.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

namespace mcoptions {

double price_american_option(Context& ctx, const AmericanOptionData& option) {
    size_t num_paths = ctx.num_simulations;
    size_t num_exercise = option.num_exercise_points;
    
    std::vector<std::vector<double>> all_paths(num_paths);
    
    for (size_t i = 0; i < num_paths; ++i) {
        auto normals = generate_normal_samples(ctx.rng, ctx.num_steps);
        all_paths[i] = simulate_gbm_path(ctx, option.spot, option.rate, option.volatility,
                                          option.time_to_maturity, ctx.num_steps, normals);
    }
    
    double dt = option.time_to_maturity / num_exercise;
    std::vector<double> cashflows(num_paths);
    
    for (size_t i = 0; i < num_paths; ++i) {
        double terminal_spot = all_paths[i].back();
        cashflows[i] = payoff(terminal_spot, option.strike, option.type);
    }
    
    for (int t = num_exercise - 1; t >= 1; --t) {
        size_t step_idx = (t * ctx.num_steps) / num_exercise;
        
        std::vector<double> X, Y;
        for (size_t i = 0; i < num_paths; ++i) {
            double spot = all_paths[i][step_idx];
            double immediate = payoff(spot, option.strike, option.type);
            
            if (immediate > 0.0) {
                X.push_back(spot);
                Y.push_back(cashflows[i] * std::exp(-option.rate * dt));
            }
        }
        
        if (X.size() < 3) continue;
        
        double mean_x = 0.0;
        for (double x : X) mean_x += x;
        mean_x /= X.size();
        
        double mean_y = 0.0;
        for (double y : Y) mean_y += y;
        mean_y /= Y.size();
        
        double L0 = 1.0;
        double L1_x = 0.0, L1_x2 = 0.0;
        double L2_x = 0.0, L2_x2 = 0.0, L2_x3 = 0.0, L2_x4 = 0.0;
        
        for (double x : X) {
            L1_x += x;
            L1_x2 += x * x;
            L2_x += x;
            L2_x2 += x * x;
            L2_x3 += x * x * x;
            L2_x4 += x * x * x * x;
        }
        
        std::vector<double> L0_vals(X.size(), 1.0);
        std::vector<double> L1_vals(X.size());
        std::vector<double> L2_vals(X.size());
        
        for (size_t i = 0; i < X.size(); ++i) {
            L1_vals[i] = 1.0 - X[i];
            L2_vals[i] = 1.0 - 2.0 * X[i] + 0.5 * X[i] * X[i];
        }
        
        double beta0 = mean_y;
        double beta1 = 0.0, beta2 = 0.0;
        
        double sum_L1Y = 0.0, sum_L1L1 = 0.0;
        for (size_t i = 0; i < X.size(); ++i) {
            sum_L1Y += L1_vals[i] * Y[i];
            sum_L1L1 += L1_vals[i] * L1_vals[i];
        }
        if (sum_L1L1 > 1e-10) {
            beta1 = sum_L1Y / sum_L1L1;
        }
        
        double sum_L2Y = 0.0, sum_L2L2 = 0.0;
        for (size_t i = 0; i < X.size(); ++i) {
            sum_L2Y += L2_vals[i] * Y[i];
            sum_L2L2 += L2_vals[i] * L2_vals[i];
        }
        if (sum_L2L2 > 1e-10) {
            beta2 = sum_L2Y / sum_L2L2;
        }
        
        size_t j = 0;
        for (size_t i = 0; i < num_paths; ++i) {
            double spot = all_paths[i][step_idx];
            double immediate = payoff(spot, option.strike, option.type);
            
            if (immediate > 0.0) {
                double continuation = beta0 + beta1 * L1_vals[j] + beta2 * L2_vals[j];
                
                if (immediate > continuation) {
                    cashflows[i] = immediate;
                } else {
                    cashflows[i] *= std::exp(-option.rate * dt);
                }
                ++j;
            } else {
                cashflows[i] *= std::exp(-option.rate * dt);
            }
        }
    }
    
    double sum_cashflows = 0.0;
    for (double cf : cashflows) {
        sum_cashflows += cf;
    }
    
    return std::exp(-option.rate * dt) * (sum_cashflows / num_paths);
}

}
