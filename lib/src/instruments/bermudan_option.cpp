#include "internal/instruments/bermudan_option.hpp"
#include "internal/methods/monte_carlo.hpp"
#include "internal/random.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

namespace mcoptions {

double price_bermudan_option(Context& ctx, const BermudanOptionData& option) {
    // Similar to American but only exercise on specific dates
    size_t num_paths = ctx.num_simulations;
    size_t num_exercise_dates = option.exercise_dates.size();
    
    if (num_exercise_dates == 0) {
        // No exercise dates - treat as European
        OptionData european{option.spot, option.strike, option.rate, 
                           option.volatility, option.time_to_maturity, option.type};
        double final_payoff = 0.0;
        for (size_t i = 0; i < num_paths; ++i) {
            auto normals = generate_normal_samples(ctx.rng, ctx.num_steps);
            auto path = simulate_gbm_path(ctx, option.spot, option.rate, option.volatility,
                                           option.time_to_maturity, ctx.num_steps, normals);
            final_payoff += payoff(path.back(), option.strike, option.type);
        }
        return discount_factor(option.rate, option.time_to_maturity) * (final_payoff / num_paths);
    }
    
    // Generate all paths
    std::vector<std::vector<double>> all_paths(num_paths);
    for (size_t i = 0; i < num_paths; ++i) {
        auto normals = generate_normal_samples(ctx.rng, ctx.num_steps);
        all_paths[i] = simulate_gbm_path(ctx, option.spot, option.rate, option.volatility,
                                          option.time_to_maturity, ctx.num_steps, normals);
    }
    
    // Map exercise dates to step indices
    std::vector<size_t> exercise_steps;
    for (double ex_date : option.exercise_dates) {
        size_t step = static_cast<size_t>((ex_date / option.time_to_maturity) * ctx.num_steps);
        exercise_steps.push_back(step);
    }
    
    // Initialize cashflows at maturity
    std::vector<double> cashflows(num_paths);
    for (size_t i = 0; i < num_paths; ++i) {
        double terminal_spot = all_paths[i].back();
        cashflows[i] = payoff(terminal_spot, option.strike, option.type);
    }
    
    // Backward induction through exercise dates (LSM algorithm)
    for (int t = num_exercise_dates - 1; t >= 0; --t) {
        size_t step_idx = exercise_steps[t];
        double time_to_ex = option.exercise_dates[t];
        double dt = (t < static_cast<int>(num_exercise_dates) - 1) 
                    ? (option.exercise_dates[t + 1] - option.exercise_dates[t])
                    : (option.time_to_maturity - option.exercise_dates[t]);
        
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
        
        // Simple regression with Laguerre polynomials (L0, L1, L2)
        double mean_y = 0.0;
        for (double y : Y) mean_y += y;
        mean_y /= Y.size();
        
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
        if (sum_L1L1 > 1e-10) beta1 = sum_L1Y / sum_L1L1;
        
        double sum_L2Y = 0.0, sum_L2L2 = 0.0;
        for (size_t i = 0; i < X.size(); ++i) {
            sum_L2Y += L2_vals[i] * Y[i];
            sum_L2L2 += L2_vals[i] * L2_vals[i];
        }
        if (sum_L2L2 > 1e-10) beta2 = sum_L2Y / sum_L2L2;
        
        // Exercise decision
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
    
    // Discount back to present from first exercise date
    double sum_cashflows = 0.0;
    for (double cf : cashflows) {
        sum_cashflows += cf;
    }
    
    double first_ex_date = option.exercise_dates[0];
    return std::exp(-option.rate * first_ex_date) * (sum_cashflows / num_paths);
}

}
