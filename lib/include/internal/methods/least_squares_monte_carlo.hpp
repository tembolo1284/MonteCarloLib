#ifndef MCOPTIONS_LEAST_SQUARES_MONTE_CARLO_HPP
#define MCOPTIONS_LEAST_SQUARES_MONTE_CARLO_HPP

#include "internal/context.hpp"
#include "internal/instruments/instrument.hpp"
#include <vector>

namespace mcoptions {

/**
 * Least Squares Monte Carlo (LSM) for American Option Pricing
 * 
 * The LSM method (Longstaff-Schwartz 2001) prices American options by:
 * 1. Simulating forward price paths using Monte Carlo
 * 2. Working backwards from maturity
 * 3. Using regression to estimate continuation value at each time step
 * 4. Comparing continuation vs immediate exercise to determine optimal strategy
 * 
 * Regression uses basis functions (powers of stock price):
 * - Constant: 1
 * - Linear: S
 * - Quadratic: S^2
 * - Cubic: S^3 (optional)
 * 
 * The continuation value is estimated as:
 *   C(S) = β₀ + β₁·S + β₂·S² + β₃·S³
 * 
 * Benefits:
 * - Natural extension of Monte Carlo
 * - Handles path-dependent features
 * - Works for high-dimensional problems
 * 
 * Limitations:
 * - Less accurate than binomial for simple American options
 * - Requires many paths (10,000+) for stability
 * - Regression can be noisy with few in-the-money paths
 */

class LeastSquaresMonteCarlo {
public:
    /**
     * Construct LSM pricer
     * 
     * @param ctx Context with simulation configuration
     * @param spot Current stock price
     * @param strike Strike price
     * @param rate Risk-free interest rate
     * @param volatility Stock volatility
     * @param time_to_maturity Time to expiration in years
     * @param is_call True for call, false for put
     * @param num_exercise_dates Number of possible exercise dates (excluding maturity)
     */
    LeastSquaresMonteCarlo(
        Context& ctx,
        double spot,
        double strike,
        double rate,
        double volatility,
        double time_to_maturity,
        bool is_call,
        size_t num_exercise_dates = 50
    );
    
    ~LeastSquaresMonteCarlo() = default;
    
    /**
     * Price the American option using LSM algorithm
     * 
     * Algorithm:
     * 1. Generate all price paths forward
     * 2. Initialize cash flows at maturity
     * 3. For each exercise date (working backwards):
     *    a. Identify in-the-money paths
     *    b. Regress discounted cash flows on stock prices
     *    c. Compare continuation value vs exercise value
     *    d. Update cash flows and exercise decisions
     * 4. Discount all cash flows to present
     * 5. Return average across all paths
     * 
     * @return American option price
     */
    double price();
    
    /**
     * Get the average optimal exercise time across all paths
     * Useful for understanding early exercise behavior
     * 
     * @return Average exercise time in years
     */
    double get_average_exercise_time() const;
    
    /**
     * Get percentage of paths exercised early
     * 
     * @return Percentage [0, 100] of paths exercised before maturity
     */
    double get_early_exercise_percentage() const;

private:
    Context& ctx_;
    double spot_;
    double strike_;
    double rate_;
    double volatility_;
    double time_to_maturity_;
    bool is_call_;
    size_t num_exercise_dates_;
    
    // Derived parameters
    size_t num_paths_;
    size_t total_steps_;  // num_exercise_dates + 1 (includes maturity)
    double dt_;           // Time step
    
    // Simulation results
    std::vector<std::vector<double>> price_paths_;  // [path][time_step]
    std::vector<double> cash_flows_;                 // Discounted cash flow for each path
    std::vector<size_t> exercise_times_;             // Exercise time step for each path
    
    /**
     * Generate all forward price paths using GBM
     */
    void generate_price_paths();
    
    /**
     * Calculate payoff at a given stock price
     */
    double calculate_payoff(double stock_price) const;
    
    /**
     * Calculate intrinsic value (immediate exercise value)
     */
    double calculate_intrinsic_value(double stock_price) const;
    
    /**
     * Perform least squares regression to estimate continuation value
     * 
     * Uses polynomial basis functions: 1, S, S², S³
     * 
     * @param stock_prices Vector of stock prices (X in regression)
     * @param discounted_cash_flows Vector of discounted future cash flows (Y in regression)
     * @param regression_coefficients Output: regression coefficients [β₀, β₁, β₂, β₃]
     */
    void least_squares_regression(
        const std::vector<double>& stock_prices,
        const std::vector<double>& discounted_cash_flows,
        std::vector<double>& regression_coefficients
    ) const;
    
    /**
     * Evaluate continuation value using regression coefficients
     * 
     * C(S) = β₀ + β₁·S + β₂·S² + β₃·S³
     * 
     * @param stock_price Current stock price
     * @param coefficients Regression coefficients from least_squares_regression
     * @return Estimated continuation value
     */
    double evaluate_continuation_value(
        double stock_price,
        const std::vector<double>& coefficients
    ) const;
    
    /**
     * Backward induction through the tree of paths
     * This is the core LSM algorithm
     */
    void backward_induction();
};

// ============================================================================
// Convenience Functions
// ============================================================================

/**
 * Price an American call option using LSM method
 * 
 * @param ctx Context with Monte Carlo configuration
 * @param option Option data
 * @param num_exercise_dates Number of possible exercise dates (default: 50)
 * @return American call price
 */
double price_american_call_lsm(
    Context& ctx,
    const OptionData& option,
    size_t num_exercise_dates = 50
);

/**
 * Price an American put option using LSM method
 * 
 * @param ctx Context with Monte Carlo configuration
 * @param option Option data
 * @param num_exercise_dates Number of possible exercise dates (default: 50)
 * @return American put price
 */
double price_american_put_lsm(
    Context& ctx,
    const OptionData& option,
    size_t num_exercise_dates = 50
);

/**
 * Price an American option using LSM method (call or put)
 * 
 * @param ctx Context with Monte Carlo configuration
 * @param option Option data (is_call determines type)
 * @param num_exercise_dates Number of possible exercise dates (default: 50)
 * @return American option price
 */
double price_american_option_lsm(
    Context& ctx,
    const OptionData& option,
    size_t num_exercise_dates = 50
);

} // namespace mcoptions

#endif // MCOPTIONS_LEAST_SQUARES_MONTE_CARLO_HPP
