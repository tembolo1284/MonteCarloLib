#include "internal/methods/least_squares_monte_carlo.hpp"
#include "internal/models/gbm.hpp"
#include "internal/random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <numeric>

namespace mcoptions {

// ============================================================================
// LeastSquaresMonteCarlo Implementation
// ============================================================================

LeastSquaresMonteCarlo::LeastSquaresMonteCarlo(
    Context& ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity,
    bool is_call,
    size_t num_exercise_dates
)
    : ctx_(ctx),
      spot_(spot),
      strike_(strike),
      rate_(rate),
      volatility_(volatility),
      time_to_maturity_(time_to_maturity),
      is_call_(is_call),
      num_exercise_dates_(num_exercise_dates)
{
    // Validate inputs
    if (spot <= 0.0) {
        throw std::invalid_argument("Spot price must be positive");
    }
    if (strike <= 0.0) {
        throw std::invalid_argument("Strike price must be positive");
    }
    if (volatility < 0.0) {
        throw std::invalid_argument("Volatility cannot be negative");
    }
    if (time_to_maturity <= 0.0) {
        throw std::invalid_argument("Time to maturity must be positive");
    }
    if (num_exercise_dates == 0) {
        throw std::invalid_argument("Must have at least one exercise date");
    }
    
    num_paths_ = ctx_.get_num_simulations();
    total_steps_ = num_exercise_dates_ + 1;  // +1 for maturity
    dt_ = time_to_maturity_ / static_cast<double>(total_steps_);
    
    // Allocate memory
    price_paths_.resize(num_paths_);
    for (auto& path : price_paths_) {
        path.resize(total_steps_ + 1);  // +1 for initial spot
    }
    
    cash_flows_.resize(num_paths_, 0.0);
    exercise_times_.resize(num_paths_, total_steps_);  // Default: exercise at maturity
}

double LeastSquaresMonteCarlo::calculate_payoff(double stock_price) const {
    if (is_call_) {
        return std::max(stock_price - strike_, 0.0);
    } else {
        return std::max(strike_ - stock_price, 0.0);
    }
}

double LeastSquaresMonteCarlo::calculate_intrinsic_value(double stock_price) const {
    return calculate_payoff(stock_price);
}

void LeastSquaresMonteCarlo::generate_price_paths() {
    auto& rng = ctx_.get_rng();
    std::normal_distribution<double> normal(0.0, 1.0);
    
    // Generate paths
    for (size_t path = 0; path < num_paths_; ++path) {
        price_paths_[path][0] = spot_;
        
        // Generate random normals for this path
        std::vector<double> random_normals(total_steps_);
        for (size_t step = 0; step < total_steps_; ++step) {
            random_normals[step] = normal(rng);
        }
        
        // Apply antithetic variates if enabled
        if (ctx_.get_antithetic() && path < num_paths_ / 2) {
            // Store for antithetic pair
            for (size_t step = 0; step < total_steps_; ++step) {
                // Will be negated for antithetic path
            }
        } else if (ctx_.get_antithetic() && path >= num_paths_ / 2) {
            // Use negated normals from paired path
            size_t paired_path = path - num_paths_ / 2;
            for (size_t step = 0; step < total_steps_; ++step) {
                random_normals[step] = -random_normals[step];
            }
        }
        
        // Simulate GBM path step by step
        for (size_t step = 0; step < total_steps_; ++step) {
            double S = price_paths_[path][step];
            double Z = random_normals[step];
            
            // GBM: S(t+dt) = S(t) * exp((r - 0.5*σ²)*dt + σ*√dt*Z)
            double drift = (rate_ - 0.5 * volatility_ * volatility_) * dt_;
            double diffusion = volatility_ * std::sqrt(dt_) * Z;
            
            price_paths_[path][step + 1] = S * std::exp(drift + diffusion);
        }
    }
}

void LeastSquaresMonteCarlo::least_squares_regression(
    const std::vector<double>& stock_prices,
    const std::vector<double>& discounted_cash_flows,
    std::vector<double>& regression_coefficients
) const {
    size_t n = stock_prices.size();
    
    if (n == 0) {
        regression_coefficients.assign(4, 0.0);
        return;
    }
    
    if (n < 4) {
        // Not enough data points - use simple average
        double avg = std::accumulate(discounted_cash_flows.begin(), 
                                     discounted_cash_flows.end(), 0.0) / n;
        regression_coefficients = {avg, 0.0, 0.0, 0.0};
        return;
    }
    
    // Build design matrix X and response vector Y
    // Basis functions: 1, S, S², S³
    std::vector<std::vector<double>> X(n, std::vector<double>(4));
    std::vector<double> Y = discounted_cash_flows;
    
    for (size_t i = 0; i < n; ++i) {
        double S = stock_prices[i];
        X[i][0] = 1.0;           // Constant
        X[i][1] = S;             // Linear
        X[i][2] = S * S;         // Quadratic
        X[i][3] = S * S * S;     // Cubic
    }
    
    // Solve normal equations: (X'X)β = X'Y
    // Build X'X (4x4 matrix)
    std::vector<std::vector<double>> XtX(4, std::vector<double>(4, 0.0));
    std::vector<double> XtY(4, 0.0);
    
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            for (size_t k = 0; k < n; ++k) {
                XtX[i][j] += X[k][i] * X[k][j];
            }
        }
        for (size_t k = 0; k < n; ++k) {
            XtY[i] += X[k][i] * Y[k];
        }
    }
    
    // Solve using Gaussian elimination with partial pivoting
    // Simple 4x4 system - direct solution
    std::vector<std::vector<double>> A = XtX;
    std::vector<double> b = XtY;
    
    // Forward elimination
    for (size_t i = 0; i < 3; ++i) {
        // Find pivot
        size_t pivot = i;
        for (size_t k = i + 1; k < 4; ++k) {
            if (std::abs(A[k][i]) > std::abs(A[pivot][i])) {
                pivot = k;
            }
        }
        
        // Swap rows
        if (pivot != i) {
            std::swap(A[i], A[pivot]);
            std::swap(b[i], b[pivot]);
        }
        
        // Check for singular matrix
        if (std::abs(A[i][i]) < 1e-10) {
            regression_coefficients.assign(4, 0.0);
            return;
        }
        
        // Eliminate
        for (size_t k = i + 1; k < 4; ++k) {
            double factor = A[k][i] / A[i][i];
            for (size_t j = i; j < 4; ++j) {
                A[k][j] -= factor * A[i][j];
            }
            b[k] -= factor * b[i];
        }
    }
    
    // Back substitution
    regression_coefficients.resize(4);
    for (int i = 3; i >= 0; --i) {
        double sum = b[i];
        for (size_t j = i + 1; j < 4; ++j) {
            sum -= A[i][j] * regression_coefficients[j];
        }
        regression_coefficients[i] = sum / A[i][i];
    }
}

double LeastSquaresMonteCarlo::evaluate_continuation_value(
    double stock_price,
    const std::vector<double>& coefficients
) const {
    if (coefficients.size() != 4) {
        return 0.0;
    }
    
    // C(S) = β₀ + β₁·S + β₂·S² + β₃·S³
    double S = stock_price;
    return coefficients[0] 
         + coefficients[1] * S
         + coefficients[2] * S * S
         + coefficients[3] * S * S * S;
}

void LeastSquaresMonteCarlo::backward_induction() {
    double discount_per_step = std::exp(-rate_ * dt_);
    
    // Step 1: Initialize cash flows at maturity
    for (size_t path = 0; path < num_paths_; ++path) {
        double terminal_price = price_paths_[path][total_steps_];
        cash_flows_[path] = calculate_payoff(terminal_price);
        exercise_times_[path] = total_steps_;
    }
    
    // Step 2: Work backwards through exercise dates
    for (int t = static_cast<int>(total_steps_) - 1; t > 0; --t) {
        size_t time_step = static_cast<size_t>(t);
        
        // Discount existing cash flows
        for (size_t path = 0; path < num_paths_; ++path) {
            cash_flows_[path] *= discount_per_step;
        }
        
        // Collect in-the-money paths for regression
        std::vector<double> itm_stock_prices;
        std::vector<double> itm_cash_flows;
        std::vector<size_t> itm_path_indices;
        
        for (size_t path = 0; path < num_paths_; ++path) {
            double stock_price = price_paths_[path][time_step];
            double intrinsic = calculate_intrinsic_value(stock_price);
            
            if (intrinsic > 0.0) {  // In-the-money
                itm_stock_prices.push_back(stock_price);
                itm_cash_flows.push_back(cash_flows_[path]);
                itm_path_indices.push_back(path);
            }
        }
        
        // Perform regression if we have enough ITM paths
        if (itm_stock_prices.size() >= 4) {
            std::vector<double> coefficients;
            least_squares_regression(itm_stock_prices, itm_cash_flows, coefficients);
            
            // For each ITM path, decide: exercise now or continue?
            for (size_t i = 0; i < itm_path_indices.size(); ++i) {
                size_t path = itm_path_indices[i];
                double stock_price = itm_stock_prices[i];
                
                double continuation_value = evaluate_continuation_value(stock_price, coefficients);
                double exercise_value = calculate_intrinsic_value(stock_price);
                
                // Exercise if immediate value exceeds continuation value
                if (exercise_value > continuation_value) {
                    cash_flows_[path] = exercise_value;
                    exercise_times_[path] = time_step;
                }
            }
        } else {
            // Not enough ITM paths for regression
            // Simple rule: exercise if deep ITM (intrinsic > 20% of strike)
            for (size_t path = 0; path < num_paths_; ++path) {
                double stock_price = price_paths_[path][time_step];
                double intrinsic = calculate_intrinsic_value(stock_price);
                
                if (intrinsic > 0.2 * strike_) {
                    cash_flows_[path] = intrinsic;
                    exercise_times_[path] = time_step;
                }
            }
        }
    }
    
    // Final discount to present value
    double remaining_discount = std::exp(-rate_ * dt_);
    for (size_t path = 0; path < num_paths_; ++path) {
        cash_flows_[path] *= remaining_discount;
    }
}

double LeastSquaresMonteCarlo::price() {
    // Step 1: Generate all price paths
    generate_price_paths();
    
    // Step 2: Backward induction with regression
    backward_induction();
    
    // Step 3: Average cash flows across all paths
    double sum = std::accumulate(cash_flows_.begin(), cash_flows_.end(), 0.0);
    return sum / static_cast<double>(num_paths_);
}

double LeastSquaresMonteCarlo::get_average_exercise_time() const {
    double sum = 0.0;
    for (size_t exercise_time : exercise_times_) {
        sum += exercise_time * dt_;
    }
    return sum / static_cast<double>(num_paths_);
}

double LeastSquaresMonteCarlo::get_early_exercise_percentage() const {
    size_t early_exercise_count = 0;
    for (size_t exercise_time : exercise_times_) {
        if (exercise_time < total_steps_) {
            early_exercise_count++;
        }
    }
    return 100.0 * static_cast<double>(early_exercise_count) / static_cast<double>(num_paths_);
}

// ============================================================================
// Convenience Functions
// ============================================================================

double price_american_call_lsm(
    Context& ctx,
    const OptionData& option,
    size_t num_exercise_dates
) {
    LeastSquaresMonteCarlo lsm(
        ctx,
        option.spot,
        option.strike,
        option.rate,
        option.volatility,
        option.time_to_maturity,
        true,  // is_call
        num_exercise_dates
    );
    
    return lsm.price();
}

double price_american_put_lsm(
    Context& ctx,
    const OptionData& option,
    size_t num_exercise_dates
) {
    LeastSquaresMonteCarlo lsm(
        ctx,
        option.spot,
        option.strike,
        option.rate,
        option.volatility,
        option.time_to_maturity,
        false,  // is_call = false
        num_exercise_dates
    );
    
    return lsm.price();
}

double price_american_option_lsm(
    Context& ctx,
    const OptionData& option,
    size_t num_exercise_dates
) {
    if (option.is_call) {
        return price_american_call_lsm(ctx, option, num_exercise_dates);
    } else {
        return price_american_put_lsm(ctx, option, num_exercise_dates);
    }
}

} // namespace mcoptions
