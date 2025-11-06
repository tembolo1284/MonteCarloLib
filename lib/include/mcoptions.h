#ifndef MCOPTIONS_H
#define MCOPTIONS_H

#include <stdint.h>
#include <stddef.h>

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
                                          size_t num_observations);
MCO_API double mco_asian_arithmetic_put(mco_context_t* ctx, double spot, double strike,
                                         double rate, double volatility, double time_to_maturity,
                                         size_t num_observations);

MCO_API double mco_american_call(mco_context_t* ctx, double spot, double strike,
                                  double rate, double volatility, double time_to_maturity,
                                  size_t num_exercise_points);
MCO_API double mco_american_put(mco_context_t* ctx, double spot, double strike,
                                 double rate, double volatility, double time_to_maturity,
                                 size_t num_exercise_points);

MCO_API double mco_bermudan_call(mco_context_t* ctx, double spot, double strike, 
                                 double rate, double volatility, 
                                 const double* exercise_dates, size_t num_dates);

MCO_API double mco_barrier_call(mco_context_t* ctx, double spot, double strike,
                               double rate, double volatility, double time_to_maturity,
                               double barrier_level, int barrier_type, double rebate);

MCO_API double mco_lookback_call(mco_context_t* ctx, double spot, double strike,
                                double rate, double volatility, double time_to_maturity,
                                int fixed_strike);

/* Bermudan Put */
MCO_API double mco_bermudan_put(mco_context_t* ctx, double spot, double strike, 
                                double rate, double volatility, 
                                const double* exercise_dates, size_t num_dates);

/* Barrier Put */
MCO_API double mco_barrier_put(mco_context_t* ctx, double spot, double strike,
                               double rate, double volatility, double time_to_maturity,
                               double barrier_level, int barrier_type, double rebate);

/* Lookback Put */
MCO_API double mco_lookback_put(mco_context_t* ctx, double spot, double strike,
                                double rate, double volatility, double time_to_maturity,
                                int fixed_strike);

// Model selection
MCO_API void mco_context_set_model(mco_context_t* ctx, int model);  // 0=GBM, 1=SABR
MCO_API void mco_context_set_sabr_params(mco_context_t* ctx, 
                                         double alpha, double beta, 
                                         double rho, double nu);

// Variance reduction
MCO_API void mco_context_set_control_variates(mco_context_t* ctx, int enabled);
MCO_API void mco_context_set_stratified_sampling(mco_context_t* ctx, int enabled);

// Future methods (placeholders - return -1.0 for "not implemented")
MCO_API double mco_european_call_fdm(mco_context_t* ctx, double spot, double strike,
                                     double rate, double volatility, double time_to_maturity);

MCO_API double mco_european_call_tree(mco_context_t* ctx, double spot, double strike,
                                      double rate, double volatility, double time_to_maturity,
                                      int num_steps);

// ============================================================================
// Least Squares Monte Carlo (LSM) for American Options (NEW)
// ============================================================================

/**
 * Price an American call option using Least Squares Monte Carlo (LSM)
 * 
 * LSM uses Monte Carlo simulation with regression to estimate continuation values
 * at each potential exercise date. This method:
 * - Simulates forward price paths
 * - Works backwards through time using regression
 * - Compares continuation value vs immediate exercise
 * - Optimal for path-dependent or high-dimensional problems
 * 
 * Requires more paths than standard MC (recommend 50,000+)
 * Configure via: mco_context_set_num_simulations()
 * 
 * @param ctx Context with Monte Carlo configuration
 * @param spot Current stock price
 * @param strike Strike price
 * @param rate Risk-free interest rate (annualized)
 * @param volatility Volatility (annualized)
 * @param time_to_maturity Time to maturity in years
 * @param num_exercise_dates Number of possible exercise dates (default: 50)
 * @return American call price
 */
double mco_lsm_american_call(
    MCOContext* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity,
    size_t num_exercise_dates
);

/**
 * Price an American put option using Least Squares Monte Carlo (LSM)
 * 
 * American puts often exhibit early exercise when deep in-the-money.
 * LSM captures this behavior through regression-based decision rules.
 * 
 * @param ctx Context with Monte Carlo configuration
 * @param spot Current stock price
 * @param strike Strike price
 * @param rate Risk-free interest rate (annualized)
 * @param volatility Volatility (annualized)
 * @param time_to_maturity Time to maturity in years
 * @param num_exercise_dates Number of possible exercise dates (default: 50)
 * @return American put price
 */
double mco_lsm_american_put(
    MCOContext* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity,
    size_t num_exercise_dates
);

/**
 * Price an American call with default number of exercise dates (50)
 * Convenience function for quick usage
 */
double mco_lsm_american_call_default(
    MCOContext* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity
);

/**
 * Price an American put with default number of exercise dates (50)
 * Convenience function for quick usage
 */
double mco_lsm_american_put_default(
    MCOContext* ctx,
    double spot,
    double strike,
    double rate,
    double volatility,
    double time_to_maturity
);


#ifdef __cplusplus
}
#endif

#endif
