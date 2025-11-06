#include "internal/methods/binomial_tree.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace mcoptions {

// ============================================================================
// BinomialTree Class Implementation
// ============================================================================

BinomialTree::BinomialTree(
    double spot,
    double rate,
    double volatility,
    double time_to_maturity,
    size_t num_steps
)
    : num_steps_(num_steps),
      spot_(spot),
      rate_(rate),
      volatility_(volatility),
      time_to_maturity_(time_to_maturity)
{
    // Validate inputs
    if (spot <= 0.0) {
        throw std::invalid_argument("Spot price must be positive");
    }
    if (volatility < 0.0) {
        throw std::invalid_argument("Volatility cannot be negative");
    }
    if (time_to_maturity <= 0.0) {
        throw std::invalid_argument("Time to maturity must be positive");
    }
    if (num_steps == 0) {
        throw std::invalid_argument("Number of steps must be positive");
    }
    
    // Calculate time step
    dt_ = time_to_maturity_ / static_cast<double>(num_steps_);
    
    // Cox-Ross-Rubinstein (CRR) parameters
    u_ = std::exp(volatility_ * std::sqrt(dt_));
    d_ = 1.0 / u_;
    
    // Risk-neutral probability
    double a = std::exp(rate_ * dt_);
    p_ = (a - d_) / (u_ - d_);
    
    // Validate risk-neutral probability
    if (p_ < 0.0 || p_ > 1.0) {
        throw std::runtime_error("Invalid risk-neutral probability: check inputs");
    }
    
    // Discount factor per time step
    discount_ = std::exp(-rate_ * dt_);
    
    // Allocate memory for option values
    // We need space for up to num_steps + 1 nodes at the final time step
    option_values_.resize(num_steps_ + 1);
    option_values_prev_.resize(num_steps_ + 1);
}

double BinomialTree::get_stock_price(size_t step, size_t up_moves) const {
    if (step > num_steps_) {
        throw std::invalid_argument("Step index out of bounds");
    }
    if (up_moves > step) {
        throw std::invalid_argument("Number of up moves cannot exceed step index");
    }
    
    // Stock price at node (step, up_moves) = S0 * u^up_moves * d^(step - up_moves)
    size_t down_moves = step - up_moves;
    return spot_ * std::pow(u_, up_moves) * std::pow(d_, down_moves);
}

double BinomialTree::calculate_payoff(bool is_call, double strike, double stock_price) const {
    if (is_call) {
        return std::max(0.0, stock_price - strike);
    } else {
        return std::max(0.0, strike - stock_price);
    }
}

double BinomialTree::calculate_intrinsic_value(bool is_call, double strike, double stock_price) const {
    // For options, intrinsic value is the same as payoff
    return calculate_payoff(is_call, strike, stock_price);
}

void BinomialTree::backward_induction(bool is_call, double strike, bool allow_early_exercise) {
    // Step 1: Initialize option values at maturity (final time step)
    for (size_t j = 0; j <= num_steps_; ++j) {
        double stock_price = get_stock_price(num_steps_, j);
        option_values_[j] = calculate_payoff(is_call, strike, stock_price);
    }
    
    // Step 2: Work backwards through the tree
    for (int step = static_cast<int>(num_steps_) - 1; step >= 0; --step) {
        size_t t = static_cast<size_t>(step);
        
        // Swap buffers (previous becomes current)
        std::swap(option_values_, option_values_prev_);
        
        // At time step t, we have t+1 possible nodes
        for (size_t j = 0; j <= t; ++j) {
            // Calculate continuation value (discounted expected value)
            // E[V] = p * V_up + (1-p) * V_down
            double continuation_value = discount_ * (
                p_ * option_values_prev_[j + 1] +  // Value if stock goes up
                (1.0 - p_) * option_values_prev_[j]  // Value if stock goes down
            );
            
            if (allow_early_exercise) {
                // For American options, compare with immediate exercise
                double stock_price = get_stock_price(t, j);
                double exercise_value = calculate_intrinsic_value(is_call, strike, stock_price);
                
                // Take the maximum of holding vs exercising
                option_values_[j] = std::max(continuation_value, exercise_value);
            } else {
                // For European options, can only use continuation value
                option_values_[j] = continuation_value;
            }
        }
    }
}

double BinomialTree::price_european(bool is_call, double strike) {
    // Perform backward induction without early exercise
    backward_induction(is_call, strike, false);
    
    // The option value at the root (t=0, j=0) is our price
    return option_values_[0];
}

double BinomialTree::price_american(bool is_call, double strike) {
    // Perform backward induction with early exercise
    backward_induction(is_call, strike, true);
    
    // The option value at the root (t=0, j=0) is our price
    return option_values_[0];
}

// ============================================================================
// Convenience Functions
// ============================================================================

double price_european_option_binomial(
    const Context& ctx,
    const OptionData& option
) {
    size_t num_steps = ctx.get_binomial_steps();
    return price_european_option_binomial(ctx, option, num_steps);
}

double price_american_option_binomial(
    const Context& ctx,
    const OptionData& option
) {
    size_t num_steps = ctx.get_binomial_steps();
    return price_american_option_binomial(ctx, option, num_steps);
}

double price_european_option_binomial(
    const Context& ctx,
    const OptionData& option,
    size_t num_steps
) {
    // Create binomial tree
    BinomialTree tree(
        option.spot,
        option.rate,
        option.volatility,
        option.time_to_maturity,
        num_steps
    );
    
    // Determine if call or put
    bool is_call = (option.type == OptionType::Call);
    
    // Price as European option
    return tree.price_european(is_call, option.strike);
}

double price_american_option_binomial(
    const Context& ctx,
    const OptionData& option,
    size_t num_steps
) {
    // Create binomial tree
    BinomialTree tree(
        option.spot,
        option.rate,
        option.volatility,
        option.time_to_maturity,
        num_steps
    );
    
    // Determine if call or put
    bool is_call = (option.type == OptionType::Call);
    
    // Price as American option (with early exercise)
    return tree.price_american(is_call, option.strike);
}

} // namespace mcoptions
