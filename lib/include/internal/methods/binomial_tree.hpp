#ifndef MCOPTIONS_BINOMIAL_TREE_HPP
#define MCOPTIONS_BINOMIAL_TREE_HPP

#include "internal/context.hpp"
#include "internal/instruments/instrument.hpp"
#include <vector>
#include <memory>

namespace mcoptions {

/**
 * Binomial Tree for option pricing using Cox-Ross-Rubinstein (CRR) model
 * 
 * The binomial tree method discretizes the continuous price process into
 * a finite number of time steps. At each node, the stock can move up or down.
 * 
 * CRR Parameters:
 * - u (up factor) = exp(sigma * sqrt(dt))
 * - d (down factor) = 1/u = exp(-sigma * sqrt(dt))
 * - p (risk-neutral probability) = (exp(r*dt) - d) / (u - d)
 * 
 * Benefits over Monte Carlo:
 * - Exact for American options (early exercise at each node)
 * - Deterministic (no random number generation)
 * - Fast for reasonable tree depths (< 500 steps)
 * - Can price path-independent options efficiently
 */
class BinomialTree {
public:
    /**
     * Construct a binomial tree for option pricing
     * 
     * @param spot Current stock price (S0)
     * @param rate Risk-free interest rate (r)
     * @param volatility Stock volatility (sigma)
     * @param time_to_maturity Time to expiration in years (T)
     * @param num_steps Number of time steps in the tree (N)
     */
    BinomialTree(
        double spot,
        double rate,
        double volatility,
        double time_to_maturity,
        size_t num_steps
    );
    
    ~BinomialTree() = default;
    
    /**
     * Price a European option (no early exercise)
     * 
     * @param is_call True for call, false for put
     * @param strike Strike price (K)
     * @return Option price
     */
    double price_european(bool is_call, double strike);
    
    /**
     * Price an American option (with early exercise)
     * 
     * American options can be exercised at any node in the tree.
     * Uses backward induction with early exercise check at each node.
     * 
     * @param is_call True for call, false for put
     * @param strike Strike price (K)
     * @return Option price
     */
    double price_american(bool is_call, double strike);
    
    /**
     * Get stock price at a specific node in the tree
     * 
     * @param step Time step index (0 to num_steps)
     * @param up_moves Number of up moves to reach this node
     * @return Stock price at node (step, up_moves)
     */
    double get_stock_price(size_t step, size_t up_moves) const;
    
    /**
     * Get the number of steps in the tree
     */
    size_t get_num_steps() const { return num_steps_; }
    
    /**
     * Get tree parameters (for debugging/validation)
     */
    double get_up_factor() const { return u_; }
    double get_down_factor() const { return d_; }
    double get_risk_neutral_prob() const { return p_; }
    double get_time_step() const { return dt_; }

private:
    // Tree parameters
    size_t num_steps_;           // Number of time steps (N)
    double dt_;                  // Time step size (T/N)
    double u_;                   // Up factor
    double d_;                   // Down factor
    double p_;                   // Risk-neutral probability of up move
    double discount_;            // Discount factor per step: exp(-r*dt)
    
    // Input parameters (stored for stock price calculation)
    double spot_;                // Initial stock price
    double rate_;                // Risk-free rate
    double volatility_;          // Volatility
    double time_to_maturity_;    // Time to maturity
    
    // Working memory for option values at each node
    // We only need to store one time step at a time (current and previous)
    std::vector<double> option_values_;      // Current time step values
    std::vector<double> option_values_prev_; // Previous time step values
    
    /**
     * Calculate the payoff at maturity (terminal nodes)
     * 
     * @param is_call True for call, false for put
     * @param strike Strike price
     * @param stock_price Stock price at terminal node
     * @return Payoff value
     */
    double calculate_payoff(bool is_call, double strike, double stock_price) const;
    
    /**
     * Calculate intrinsic value (immediate exercise value)
     * Used for early exercise check in American options
     * 
     * @param is_call True for call, false for put
     * @param strike Strike price
     * @param stock_price Current stock price
     * @return Intrinsic value (max of payoff or 0)
     */
    double calculate_intrinsic_value(bool is_call, double strike, double stock_price) const;
    
    /**
     * Perform backward induction through the tree
     * 
     * @param is_call True for call, false for put
     * @param strike Strike price
     * @param allow_early_exercise True for American, false for European
     */
    void backward_induction(bool is_call, double strike, bool allow_early_exercise);
};

// ============================================================================
// Convenience Functions (Match existing API style)
// ============================================================================

/**
 * Price a European option using binomial tree method
 * 
 * @param ctx Context containing binomial_steps configuration
 * @param option Option data (spot, strike, rate, volatility, time, is_call)
 * @return Option price
 */
double price_european_option_binomial(
    const Context& ctx,
    const OptionData& option
);

/**
 * Price an American option using binomial tree method
 * 
 * This is where binomial trees excel - they handle early exercise naturally
 * through backward induction.
 * 
 * @param ctx Context containing binomial_steps configuration
 * @param option Option data (spot, strike, rate, volatility, time, is_call)
 * @return Option price
 */
double price_american_option_binomial(
    const Context& ctx,
    const OptionData& option
);

/**
 * Price a European option with explicit tree depth override
 * 
 * @param ctx Context (for consistency)
 * @param option Option data
 * @param num_steps Number of tree steps (overrides context setting)
 * @return Option price
 */
double price_european_option_binomial(
    const Context& ctx,
    const OptionData& option,
    size_t num_steps
);

/**
 * Price an American option with explicit tree depth override
 * 
 * @param ctx Context (for consistency)
 * @param option Option data
 * @param num_steps Number of tree steps (overrides context setting)
 * @return Option price
 */
double price_american_option_binomial(
    const Context& ctx,
    const OptionData& option,
    size_t num_steps
);

} // namespace mcoptions

#endif // MCOPTIONS_BINOMIAL_TREE_HPP
