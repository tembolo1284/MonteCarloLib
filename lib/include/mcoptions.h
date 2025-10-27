#ifndef MCOPTIONS_H
#define MCOPTIONS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
    #ifdef MCOPTIONS_EXPORTS
        #define MCO_API __declspec(dllexport)
    #else
        #define MCO_API __declspec(dllimport)
    #endif
#else
    #define MCO_API __attribute__((visibility("default")))
#endif

typedef struct mco_context mco_context_t;

MCO_API mco_context_t* mco_context_new(void);
MCO_API void mco_context_free(mco_context_t* ctx);
MCO_API void mco_context_set_seed(mco_context_t* ctx, uint64_t seed);
MCO_API void mco_context_set_num_simulations(mco_context_t* ctx, uint64_t n);
MCO_API void mco_context_set_num_steps(mco_context_t* ctx, uint64_t n);
MCO_API void mco_context_set_antithetic(mco_context_t* ctx, int enabled);
MCO_API void mco_context_set_importance_sampling(mco_context_t* ctx, int enabled, double drift_shift);

MCO_API double mco_european_call(mco_context_t* ctx, double spot, double strike, 
                                  double rate, double volatility, double time_to_maturity);
MCO_API double mco_european_put(mco_context_t* ctx, double spot, double strike,
                                 double rate, double volatility, double time_to_maturity);

MCO_API double mco_asian_arithmetic_call(mco_context_t* ctx, double spot, double strike,
                                          double rate, double volatility, double time_to_maturity,
                                          int num_observations);
MCO_API double mco_asian_arithmetic_put(mco_context_t* ctx, double spot, double strike,
                                         double rate, double volatility, double time_to_maturity,
                                         int num_observations);

MCO_API double mco_american_call(mco_context_t* ctx, double spot, double strike,
                                  double rate, double volatility, double time_to_maturity,
                                  int num_exercise_points);
MCO_API double mco_american_put(mco_context_t* ctx, double spot, double strike,
                                 double rate, double volatility, double time_to_maturity,
                                 int num_exercise_points);

#ifdef __cplusplus
}
#endif

#endif
