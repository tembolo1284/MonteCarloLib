#include "mcoptions.h"
#include "internal/context.hpp"
#include "internal/methods/least_squares_monte_carlo.hpp"
#include "internal/instruments/european_option.hpp"
#include "internal/instruments/asian_option.hpp"
#include "internal/instruments/american_option.hpp"
#include "internal/instruments/bermudan_option.hpp"
#include "internal/instruments/barrier_option.hpp"
#include "internal/instruments/lookback_option.hpp"
#include "internal/instruments/instrument.hpp"

using namespace mcoptions;

// Context management
mco_context_t* mco_context_new() {
    return reinterpret_cast<mco_context_t*>(new Context());
}

void mco_context_free(mco_context_t* ctx) {
    delete reinterpret_cast<Context*>(ctx);
}

void mco_context_set_seed(mco_context_t* ctx, uint64_t seed) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->set_seed(seed);
}

void mco_context_set_num_simulations(mco_context_t* ctx, uint64_t n) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->set_num_simulations(n);
}

void mco_context_set_num_steps(mco_context_t* ctx, uint64_t n) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->set_num_steps(n);
}

void mco_context_set_antithetic(mco_context_t* ctx, int enabled) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->set_antithetic(enabled != 0);
}

void mco_context_set_importance_sampling(mco_context_t* ctx, int enabled, double drift_shift) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->set_importance_sampling(enabled != 0, drift_shift);
}

// Variance Reduction
void mco_context_set_control_variates(mco_context_t* ctx, int enabled) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->set_control_variates(enabled != 0);
}

void mco_context_set_stratified_sampling(mco_context_t* ctx, int enabled) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->set_stratified_sampling(enabled != 0);
}

// Model Selection
void mco_context_set_model(mco_context_t* ctx, int model) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->set_model(static_cast<Context::Model>(model));
}

void mco_context_set_sabr_params(mco_context_t* ctx, 
                                 double alpha, double beta, 
                                 double rho, double nu) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->set_sabr_params(alpha, beta, rho, nu);
}

// European Options
double mco_european_call(mco_context_t* ctx, double spot, double strike,
                         double rate, double volatility, double time_to_maturity) {
    Context* context = reinterpret_cast<Context*>(ctx);
    OptionData option{spot, strike, rate, volatility, time_to_maturity, OptionType::Call};
    return price_european_option(*context, option);
}

double mco_european_put(mco_context_t* ctx, double spot, double strike,
                        double rate, double volatility, double time_to_maturity) {
    Context* context = reinterpret_cast<Context*>(ctx);
    OptionData option{spot, strike, rate, volatility, time_to_maturity, OptionType::Put};
    return price_european_option(*context, option);
}

// Asian Options
double mco_asian_arithmetic_call(mco_context_t* ctx, double spot, double strike,
                                  double rate, double volatility, double time_to_maturity,
                                  size_t num_observations) {
    Context* context = reinterpret_cast<Context*>(ctx);
    AsianOptionData option{spot, strike, rate, volatility, time_to_maturity, 
                          OptionType::Call, num_observations};
    return price_asian_option(*context, option);
}

double mco_asian_arithmetic_put(mco_context_t* ctx, double spot, double strike,
                                 double rate, double volatility, double time_to_maturity,
                                 size_t num_observations) {
    Context* context = reinterpret_cast<Context*>(ctx);
    AsianOptionData option{spot, strike, rate, volatility, time_to_maturity,
                          OptionType::Put, num_observations};
    return price_asian_option(*context, option);
}

// American Options
double mco_american_call(mco_context_t* ctx, double spot, double strike,
                         double rate, double volatility, double time_to_maturity,
                         size_t num_exercise_points) {
    Context* context = reinterpret_cast<Context*>(ctx);
    AmericanOptionData option{spot, strike, rate, volatility, time_to_maturity,
                             OptionType::Call, num_exercise_points};
    return price_american_option(*context, option);
}

double mco_american_put(mco_context_t* ctx, double spot, double strike,
                        double rate, double volatility, double time_to_maturity,
                        size_t num_exercise_points) {
    Context* context = reinterpret_cast<Context*>(ctx);
    AmericanOptionData option{spot, strike, rate, volatility, time_to_maturity,
                             OptionType::Put, num_exercise_points};
    return price_american_option(*context, option);
}

// Bermudan Options
double mco_bermudan_call(mco_context_t* ctx, double spot, double strike, 
                         double rate, double volatility, 
                         const double* exercise_dates, size_t num_dates) {
    Context* context = reinterpret_cast<Context*>(ctx);
    std::vector<double> ex_dates(exercise_dates, exercise_dates + num_dates);
    BermudanOptionData option{spot, strike, rate, volatility, ex_dates.back(),
                             OptionType::Call, ex_dates};
    return price_bermudan_option(*context, option);
}

// Barrier Options
double mco_barrier_call(mco_context_t* ctx, double spot, double strike,
                        double rate, double volatility, double time_to_maturity,
                        double barrier_level, int barrier_type, double rebate) {
    Context* context = reinterpret_cast<Context*>(ctx);
    BarrierOptionData option{spot, strike, rate, volatility, time_to_maturity,
                            OptionType::Call, barrier_level, 
                            static_cast<BarrierType>(barrier_type), rebate};
    return price_barrier_option(*context, option);
}

// Lookback Options
double mco_lookback_call(mco_context_t* ctx, double spot, double strike,
                         double rate, double volatility, double time_to_maturity,
                         int fixed_strike) {
    Context* context = reinterpret_cast<Context*>(ctx);
    LookbackOptionData option{spot, strike, rate, volatility, time_to_maturity,
                             OptionType::Call, static_cast<bool>(fixed_strike)};
    return price_lookback_option(*context, option);
}

// Bermudan Put
double mco_bermudan_put(mco_context_t* ctx, double spot, double strike, 
                        double rate, double volatility, 
                        const double* exercise_dates, size_t num_dates) {
    Context* context = reinterpret_cast<Context*>(ctx);
    std::vector<double> ex_dates(exercise_dates, exercise_dates + num_dates);
    BermudanOptionData option{spot, strike, rate, volatility, ex_dates.back(),
                             OptionType::Put, ex_dates};
    return price_bermudan_option(*context, option);
}

// Barrier Put
double mco_barrier_put(mco_context_t* ctx, double spot, double strike,
                       double rate, double volatility, double time_to_maturity,
                       double barrier_level, int barrier_type, double rebate) {
    Context* context = reinterpret_cast<Context*>(ctx);
    BarrierOptionData option{spot, strike, rate, volatility, time_to_maturity,
                            OptionType::Put, barrier_level, 
                            static_cast<BarrierType>(barrier_type), rebate};
    return price_barrier_option(*context, option);
}

// Lookback Put
double mco_lookback_put(mco_context_t* ctx, double spot, double strike,
                        double rate, double volatility, double time_to_maturity,
                        int fixed_strike) {
    Context* context = reinterpret_cast<Context*>(ctx);
    LookbackOptionData option{spot, strike, rate, volatility, time_to_maturity,
                             OptionType::Put, static_cast<bool>(fixed_strike)};
    return price_lookback_option(*context, option);
}

// Finite Difference Method (STUB)
double mco_european_call_fdm(mco_context_t* ctx, double spot, double strike,
                             double rate, double volatility, double time_to_maturity) {
    return -1.0;  // Not implemented
}

// Binomial Tree Method (STUB)
double mco_european_call_tree(mco_context_t* ctx, double spot, double strike,
                              double rate, double volatility, double time_to_maturity,
                              int num_steps) {
    return -1.0;  // Not implemented
}

// ============================================================================
// Binomial Tree Methods
// ============================================================================

void mco_context_set_binomial_steps(mco_context_t* ctx, size_t n) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->set_binomial_steps(n);
}

size_t mco_context_get_binomial_steps(mco_context_t* ctx) {
    Context* context = reinterpret_cast<Context*>(ctx);
    return context->get_binomial_steps();
}

// For now, these are stubs that return -1.0 (not implemented)
// You'll implement them when you add the actual binomial tree pricing logic

double mco_binomial_european_call(
    mco_context_t* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity
) {
    Context* context = reinterpret_cast<Context*>(ctx);
    size_t steps = context->get_binomial_steps();
    return mco_binomial_european_call_steps(ctx, spot, strike, rate, volatility, time_to_maturity, steps);
}

double mco_binomial_european_put(
    mco_context_t* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity
) {
    Context* context = reinterpret_cast<Context*>(ctx);
    size_t steps = context->get_binomial_steps();
    return mco_binomial_european_put_steps(ctx, spot, strike, rate, volatility, time_to_maturity, steps);
}

double mco_binomial_american_call(
    mco_context_t* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity
) {
    Context* context = reinterpret_cast<Context*>(ctx);
    size_t steps = context->get_binomial_steps();
    return mco_binomial_american_call_steps(ctx, spot, strike, rate, volatility, time_to_maturity, steps);
}

double mco_binomial_american_put(
    mco_context_t* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity
) {
    Context* context = reinterpret_cast<Context*>(ctx);
    size_t steps = context->get_binomial_steps();
    return mco_binomial_american_put_steps(ctx, spot, strike, rate, volatility, time_to_maturity, steps);
}

// The _steps versions - these are the actual implementations (currently stubs)
double mco_binomial_european_call_steps(
    mco_context_t* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity,
    size_t num_steps
) {
    // TODO: Implement binomial tree pricing
    return -1.0;  // Not implemented yet
}

double mco_binomial_european_put_steps(
    mco_context_t* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity,
    size_t num_steps
) {
    // TODO: Implement binomial tree pricing
    return -1.0;  // Not implemented yet
}

double mco_binomial_american_call_steps(
    mco_context_t* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity,
    size_t num_steps
) {
    // TODO: Implement binomial tree pricing
    return -1.0;  // Not implemented yet
}

double mco_binomial_american_put_steps(
    mco_context_t* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity,
    size_t num_steps
) {
    // TODO: Implement binomial tree pricing
    return -1.0;  // Not implemented yet
}

// ============================================================================
// LSM American Option Pricing
// ============================================================================

double mco_lsm_american_call(
    mco_context* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity,
    size_t num_exercise_dates
) {
    if (!ctx) return -1.0;
    
    Context* context = reinterpret_cast<Context*>(ctx);
    
    OptionData option;
    option.spot = spot;
    option.strike = strike;
    option.rate = rate;
    option.volatility = volatility;
    option.time_to_maturity = time_to_maturity;
    option.type = OptionType::Call;  // Changed from is_call
    
    return price_american_call_lsm(*context, option, num_exercise_dates);
}

double mco_lsm_american_put(
    mco_context* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity,
    size_t num_exercise_dates
) {
    if (!ctx) return -1.0;
    
    Context* context = reinterpret_cast<Context*>(ctx);
    
    OptionData option;
    option.spot = spot;
    option.strike = strike;
    option.rate = rate;
    option.volatility = volatility;
    option.time_to_maturity = time_to_maturity;
    option.type = OptionType::Put;  // Changed from is_call
    
    return price_american_put_lsm(*context, option, num_exercise_dates);
}

double mco_lsm_american_call_default(
    mco_context* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity
) {
    return mco_lsm_american_call(ctx, spot, strike, rate, volatility, time_to_maturity, 50);
}

double mco_lsm_american_put_default(
    mco_context* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity
) {
    return mco_lsm_american_put(ctx, spot, strike, rate, volatility, time_to_maturity, 50);
}
