#include "mcoptions.h"
#include "internal/context.hpp"
#include "internal/european_option.hpp"
#include "internal/asian_option.hpp"
#include "internal/american_option.hpp"

using namespace mcoptions;

extern "C" {

mco_context_t* mco_context_new(void) {
    return reinterpret_cast<mco_context_t*>(new Context());
}

void mco_context_free(mco_context_t* ctx) {
    delete reinterpret_cast<Context*>(ctx);
}

void mco_context_set_seed(mco_context_t* ctx, uint64_t seed) {
    Context* c = reinterpret_cast<Context*>(ctx);
    c->seed = seed;
    c->reset_rng();
}

void mco_context_set_num_simulations(mco_context_t* ctx, uint64_t n) {
    reinterpret_cast<Context*>(ctx)->num_simulations = n;
}

void mco_context_set_num_steps(mco_context_t* ctx, uint64_t n) {
    reinterpret_cast<Context*>(ctx)->num_steps = n;
}

void mco_context_set_antithetic(mco_context_t* ctx, int enabled) {
    reinterpret_cast<Context*>(ctx)->antithetic_enabled = enabled != 0;
}

void mco_context_set_importance_sampling(mco_context_t* ctx, int enabled, double drift_shift) {
    Context* c = reinterpret_cast<Context*>(ctx);
    c->importance_sampling_enabled = enabled != 0;
    c->drift_shift = drift_shift;
}

double mco_european_call(mco_context_t* ctx, double spot, double strike,
                         double rate, double volatility, double time_to_maturity) {
    Context* c = reinterpret_cast<Context*>(ctx);
    OptionData option{spot, strike, rate, volatility, time_to_maturity, OptionType::Call};
    return price_european_option(*c, option);
}

double mco_european_put(mco_context_t* ctx, double spot, double strike,
                        double rate, double volatility, double time_to_maturity) {
    Context* c = reinterpret_cast<Context*>(ctx);
    OptionData option{spot, strike, rate, volatility, time_to_maturity, OptionType::Put};
    return price_european_option(*c, option);
}

double mco_asian_arithmetic_call(mco_context_t* ctx, double spot, double strike,
                                 double rate, double volatility, double time_to_maturity,
                                 int num_observations) {
    Context* c = reinterpret_cast<Context*>(ctx);
    AsianOptionData option;
    option.spot = spot;
    option.strike = strike;
    option.rate = rate;
    option.volatility = volatility;
    option.time_to_maturity = time_to_maturity;
    option.type = OptionType::Call;
    option.num_observations = num_observations;
    return price_asian_option(*c, option);
}

double mco_asian_arithmetic_put(mco_context_t* ctx, double spot, double strike,
                                double rate, double volatility, double time_to_maturity,
                                int num_observations) {
    Context* c = reinterpret_cast<Context*>(ctx);
    AsianOptionData option;
    option.spot = spot;
    option.strike = strike;
    option.rate = rate;
    option.volatility = volatility;
    option.time_to_maturity = time_to_maturity;
    option.type = OptionType::Put;
    option.num_observations = num_observations;
    return price_asian_option(*c, option);
}

double mco_american_call(mco_context_t* ctx, double spot, double strike,
                         double rate, double volatility, double time_to_maturity,
                         int num_exercise_points) {
    Context* c = reinterpret_cast<Context*>(ctx);
    AmericanOptionData option;
    option.spot = spot;
    option.strike = strike;
    option.rate = rate;
    option.volatility = volatility;
    option.time_to_maturity = time_to_maturity;
    option.type = OptionType::Call;
    option.num_exercise_points = num_exercise_points;
    return price_american_option(*c, option);
}

double mco_american_put(mco_context_t* ctx, double spot, double strike,
                        double rate, double volatility, double time_to_maturity,
                        int num_exercise_points) {
    Context* c = reinterpret_cast<Context*>(ctx);
    AmericanOptionData option;
    option.spot = spot;
    option.strike = strike;
    option.rate = rate;
    option.volatility = volatility;
    option.time_to_maturity = time_to_maturity;
    option.type = OptionType::Put;
    option.num_exercise_points = num_exercise_points;
    return price_american_option(*c, option);
}

}
