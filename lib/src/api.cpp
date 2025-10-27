#include "mcoptions.h"
#include "internal/context.hpp"
#include "internal/instruments/european_option.hpp"
#include "internal/instruments/asian_option.hpp"
#include "internal/instruments/american_option.hpp"
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
    context->seed = seed;
    context->reset_rng();
}

void mco_context_set_num_simulations(mco_context_t* ctx, uint64_t n) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->num_simulations = n;
}

void mco_context_set_num_steps(mco_context_t* ctx, uint64_t n) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->num_steps = n;
}

void mco_context_set_antithetic(mco_context_t* ctx, int enabled) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->antithetic_enabled = (enabled != 0);
}

void mco_context_set_importance_sampling(mco_context_t* ctx, int enabled, double drift_shift) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->importance_sampling_enabled = (enabled != 0);
    context->drift_shift = drift_shift;
}

// Variance Reduction
void mco_context_set_control_variates(mco_context_t* ctx, int enabled) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->control_variates_enabled = (enabled != 0);
}

void mco_context_set_stratified_sampling(mco_context_t* ctx, int enabled) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->stratified_sampling_enabled = (enabled != 0);
}

// Model Selection
void mco_context_set_model(mco_context_t* ctx, int model) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->model = static_cast<Context::Model>(model);
}

void mco_context_set_sabr_params(mco_context_t* ctx, 
                                 double alpha, double beta, 
                                 double rho, double nu) {
    Context* context = reinterpret_cast<Context*>(ctx);
    context->sabr_alpha = alpha;
    context->sabr_beta = beta;
    context->sabr_rho = rho;
    context->sabr_nu = nu;
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
                                  int num_observations) {
    Context* context = reinterpret_cast<Context*>(ctx);
    AsianOptionData option{spot, strike, rate, volatility, time_to_maturity, 
                          OptionType::Call, static_cast<size_t>(num_observations)};
    return price_asian_option(*context, option);
}

double mco_asian_arithmetic_put(mco_context_t* ctx, double spot, double strike,
                                 double rate, double volatility, double time_to_maturity,
                                 int num_observations) {
    Context* context = reinterpret_cast<Context*>(ctx);
    AsianOptionData option{spot, strike, rate, volatility, time_to_maturity,
                          OptionType::Put, static_cast<size_t>(num_observations)};
    return price_asian_option(*context, option);
}

// American Options
double mco_american_call(mco_context_t* ctx, double spot, double strike,
                         double rate, double volatility, double time_to_maturity,
                         int num_exercise_points) {
    Context* context = reinterpret_cast<Context*>(ctx);
    AmericanOptionData option{spot, strike, rate, volatility, time_to_maturity,
                             OptionType::Call, static_cast<size_t>(num_exercise_points)};
    return price_american_option(*context, option);
}

double mco_american_put(mco_context_t* ctx, double spot, double strike,
                        double rate, double volatility, double time_to_maturity,
                        int num_exercise_points) {
    Context* context = reinterpret_cast<Context*>(ctx);
    AmericanOptionData option{spot, strike, rate, volatility, time_to_maturity,
                             OptionType::Put, static_cast<size_t>(num_exercise_points)};
    return price_american_option(*context, option);
}

// Bermudan Options (STUB)
double mco_bermudan_call(mco_context_t* ctx, double spot, double strike, 
                         double rate, double volatility, 
                         const double* exercise_dates, size_t num_dates) {
    return -1.0;  // Not implemented
}

// Barrier Options (STUB)
double mco_barrier_call(mco_context_t* ctx, double spot, double strike,
                        double rate, double volatility, double time_to_maturity,
                        double barrier_level, int barrier_type, double rebate) {
    return -1.0;  // Not implemented
}

// Lookback Options (STUB)
double mco_lookback_call(mco_context_t* ctx, double spot, double strike,
                         double rate, double volatility, double time_to_maturity,
                         int fixed_strike) {
    return -1.0;  // Not implemented
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
