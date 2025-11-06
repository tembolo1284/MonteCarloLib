/**
 * Comprehensive American Option Pricing Comparison
 * 
 * This test compares three methods for pricing American options:
 * 1. Binomial Tree (CRR) - Exact/benchmark method
 * 2. Least Squares Monte Carlo (LSM) - Monte Carlo with regression
 * 3. Standard Monte Carlo (placeholder for now)
 * 
 * Key insights:
 * - Binomial is deterministic and exact
 * - LSM is stochastic but flexible
 * - American options worth more than European (early exercise premium)
 */

#include "mcoptions.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

// ============================================================================
// Utility Functions
// ============================================================================

double normal_cdf(double x) {
    return 0.5 * (1.0 + erf(x / sqrt(2.0)));
}

double black_scholes_call(double S, double K, double r, double sigma, double T) {
    double d1 = (log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
    double d2 = d1 - sigma * sqrt(T);
    return S * normal_cdf(d1) - K * exp(-r * T) * normal_cdf(d2);
}

double black_scholes_put(double S, double K, double r, double sigma, double T) {
    double d1 = (log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
    double d2 = d1 - sigma * sqrt(T);
    return K * exp(-r * T) * normal_cdf(-d2) - S * normal_cdf(-d1);
}

void print_separator() {
    printf("================================================================\n");
}

void print_header(const char* title) {
    print_separator();
    printf("  %s\n", title);
    print_separator();
}

// ============================================================================
// Test 1: ATM American Put (Most Common Test Case)
// ============================================================================

void test_atm_american_put() {
    print_header("Test 1: ATM American Put - Method Comparison");
    
    MCOContext* ctx = mco_context_new();
    
    // Configure for fair comparison
    mco_context_set_binomial_steps(ctx, 200);
    mco_context_set_num_simulations(ctx, 100000);
    mco_context_set_antithetic(ctx, 1);
    
    // ATM parameters
    double spot = 100.0;
    double strike = 100.0;
    double rate = 0.05;
    double volatility = 0.20;
    double time = 1.0;
    
    printf("\nOption Parameters:\n");
    printf("  Spot Price:    $%.2f\n", spot);
    printf("  Strike Price:  $%.2f\n", strike);
    printf("  Risk-free Rate: %.2f%%\n", rate * 100);
    printf("  Volatility:     %.2f%%\n", volatility * 100);
    printf("  Time to Expiry: %.2f years\n", time);
    
    printf("\nConfiguration:\n");
    printf("  Binomial Steps:     %zu\n", mco_context_get_binomial_steps(ctx));
    printf("  MC Simulations:     %zu\n", mco_context_get_num_simulations(ctx));
    printf("  LSM Exercise Dates: 50\n");
    
    // Price with all methods
    clock_t start, end;
    double cpu_time;
    
    // Binomial (benchmark)
    printf("\n--- Binomial Tree (Benchmark) ---\n");
    start = clock();
    double binomial_american = mco_binomial_american_put(ctx, spot, strike, rate, volatility, time);
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("  American Put: $%.4f (%.3f seconds)\n", binomial_american, cpu_time);
    
    double binomial_european = mco_binomial_european_put(ctx, spot, strike, rate, volatility, time);
    double early_exercise_value = binomial_american - binomial_european;
    printf("  European Put: $%.4f\n", binomial_european);
    printf("  Early Ex Premium: $%.4f (%.2f%%)\n", 
           early_exercise_value, 
           100.0 * early_exercise_value / binomial_european);
    
    // LSM
    printf("\n--- Least Squares Monte Carlo ---\n");
    start = clock();
    double lsm_american = mco_lsm_american_put_default(ctx, spot, strike, rate, volatility, time);
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("  American Put: $%.4f (%.3f seconds)\n", lsm_american, cpu_time);
    
    double lsm_error = fabs(lsm_american - binomial_american);
    double lsm_pct_error = 100.0 * lsm_error / binomial_american;
    printf("  Error vs Binomial: $%.4f (%.2f%%)\n", lsm_error, lsm_pct_error);
    
    // Standard MC European (for reference)
    printf("\n--- Standard Monte Carlo (European) ---\n");
    start = clock();
    double mc_european = mco_european_put(ctx, spot, strike, rate, volatility, time);
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("  European Put: $%.4f (%.3f seconds)\n", mc_european, cpu_time);
    
    // Black-Scholes (theoretical European)
    double bs_put = black_scholes_put(spot, strike, rate, volatility, time);
    printf("\n--- Black-Scholes (European) ---\n");
    printf("  European Put: $%.4f\n", bs_put);
    
    // Summary
    printf("\n--- Summary ---\n");
    printf("  Method              | Price   | Error    | Time\n");
    printf("  --------------------|---------|----------|--------\n");
    printf("  Binomial American   | $%.4f  | -        | Fast\n", binomial_american);
    printf("  LSM American        | $%.4f  | $%.4f   | Slow\n", lsm_american, lsm_error);
    printf("  Binomial European   | $%.4f  | -        | Fast\n", binomial_european);
    printf("  MC European         | $%.4f  | $%.4f   | Medium\n", 
           mc_european, fabs(mc_european - bs_put));
    printf("  Black-Scholes       | $%.4f  | -        | Instant\n", bs_put);
    
    printf("\n✓ Test completed\n");
    
    mco_context_free(ctx);
}

// ============================================================================
// Test 2: Deep ITM American Put (High Early Exercise)
// ============================================================================

void test_deep_itm_american_put() {
    print_header("Test 2: Deep ITM American Put - Early Exercise Behavior");
    
    MCOContext* ctx = mco_context_new();
    mco_context_set_binomial_steps(ctx, 200);
    mco_context_set_num_simulations(ctx, 100000);
    mco_context_set_antithetic(ctx, 1);
    
    // Deep ITM put
    double spot = 80.0;    // 20% below strike
    double strike = 100.0;
    double rate = 0.05;
    double volatility = 0.20;
    double time = 1.0;
    
    printf("\nOption Parameters: Deep In-The-Money\n");
    printf("  Spot Price:    $%.2f (%.0f%% of strike)\n", spot, 100.0 * spot / strike);
    printf("  Strike Price:  $%.2f\n", strike);
    printf("  Moneyness:     %.2f%% ITM\n", 100.0 * (strike - spot) / strike);
    
    double intrinsic = strike - spot;
    printf("  Intrinsic Value: $%.2f\n", intrinsic);
    
    // Price with all methods
    double binomial_american = mco_binomial_american_put(ctx, spot, strike, rate, volatility, time);
    double binomial_european = mco_binomial_european_put(ctx, spot, strike, rate, volatility, time);
    double lsm_american = mco_lsm_american_put_default(ctx, spot, strike, rate, volatility, time);
    
    printf("\n--- Pricing Results ---\n");
    printf("  Intrinsic Value:      $%.4f\n", intrinsic);
    printf("  Binomial American:    $%.4f\n", binomial_american);
    printf("  Binomial European:    $%.4f\n", binomial_european);
    printf("  LSM American:         $%.4f\n", lsm_american);
    
    double early_ex_premium = binomial_american - binomial_european;
    printf("\n--- Early Exercise Analysis ---\n");
    printf("  Early Ex Premium:     $%.4f\n", early_ex_premium);
    printf("  Premium / Intrinsic:  %.2f%%\n", 100.0 * early_ex_premium / intrinsic);
    printf("  American / European:  %.2f%%\n", 100.0 * binomial_american / binomial_european);
    
    printf("\n--- LSM Accuracy ---\n");
    double lsm_error = fabs(lsm_american - binomial_american);
    printf("  Error vs Binomial:    $%.4f (%.2f%%)\n", 
           lsm_error, 100.0 * lsm_error / binomial_american);
    
    printf("\n✓ Test completed\n");
    
    mco_context_free(ctx);
}

// ============================================================================
// Test 3: Convergence Analysis
// ============================================================================

void test_convergence_analysis() {
    print_header("Test 3: Convergence Analysis - Steps vs Accuracy");
    
    MCOContext* ctx = mco_context_new();
    
    double spot = 100.0;
    double strike = 100.0;
    double rate = 0.05;
    double volatility = 0.20;
    double time = 1.0;
    
    printf("\nOption: ATM American Put\n");
    printf("  S=$%.0f, K=$%.0f, r=%.1f%%, σ=%.0f%%, T=%.1f years\n",
           spot, strike, rate*100, volatility*100, time);
    
    // Get reference price with high accuracy binomial
    double reference = mco_binomial_american_put_steps(
        ctx, spot, strike, rate, volatility, time, 1000
    );
    
    printf("\nReference Price (1000 steps): $%.6f\n", reference);
    
    // Test binomial convergence
    printf("\n--- Binomial Tree Convergence ---\n");
    printf("Steps  | Price    | Error    | Error %%\n");
    printf("-------|----------|----------|----------\n");
    
    size_t binomial_steps[] = {10, 25, 50, 100, 200, 500};
    for (int i = 0; i < 6; i++) {
        double price = mco_binomial_american_put_steps(
            ctx, spot, strike, rate, volatility, time, binomial_steps[i]
        );
        double error = fabs(price - reference);
        double error_pct = 100.0 * error / reference;
        
        printf("%6zu | $%.6f | $%.6f | %.4f%%\n",
               binomial_steps[i], price, error, error_pct);
    }
    
    // Test LSM convergence with paths
    printf("\n--- LSM Convergence (Simulation Paths) ---\n");
    printf("Paths   | Price    | Error    | Error %%\n");
    printf("--------|----------|----------|----------\n");
    
    size_t sim_counts[] = {10000, 25000, 50000, 100000, 200000};
    for (int i = 0; i < 5; i++) {
        mco_context_set_num_simulations(ctx, sim_counts[i]);
        mco_context_set_antithetic(ctx, 1);
        
        double price = mco_lsm_american_put_default(ctx, spot, strike, rate, volatility, time);
        double error = fabs(price - reference);
        double error_pct = 100.0 * error / reference;
        
        printf("%7zu | $%.6f | $%.6f | %.4f%%\n",
               sim_counts[i], price, error, error_pct);
    }
    
    printf("\n✓ Test completed\n");
    
    mco_context_free(ctx);
}

// ============================================================================
// Test 4: American Call (Rarely Exercised Early)
// ============================================================================

void test_american_call() {
    print_header("Test 4: American Call - Early Exercise Analysis");
    
    MCOContext* ctx = mco_context_new();
    mco_context_set_binomial_steps(ctx, 200);
    mco_context_set_num_simulations(ctx, 100000);
    mco_context_set_antithetic(ctx, 1);
    
    // Test with dividends (not implemented yet, but show the concept)
    double spot = 100.0;
    double strike = 100.0;
    double rate = 0.05;
    double volatility = 0.20;
    double time = 1.0;
    
    printf("\nOption: ATM American Call (No Dividends)\n");
    printf("  S=$%.0f, K=$%.0f, r=%.1f%%, σ=%.0f%%, T=%.1f years\n",
           spot, strike, rate*100, volatility*100, time);
    
    double binomial_american = mco_binomial_american_call(ctx, spot, strike, rate, volatility, time);
    double binomial_european = mco_binomial_european_call(ctx, spot, strike, rate, volatility, time);
    double lsm_american = mco_lsm_american_call_default(ctx, spot, strike, rate, volatility, time);
    double bs_call = black_scholes_call(spot, strike, rate, volatility, time);
    
    printf("\n--- Pricing Results ---\n");
    printf("  Black-Scholes (European): $%.4f\n", bs_call);
    printf("  Binomial European:        $%.4f\n", binomial_european);
    printf("  Binomial American:        $%.4f\n", binomial_american);
    printf("  LSM American:             $%.4f\n", lsm_american);
    
    double early_ex_premium = binomial_american - binomial_european;
    
    printf("\n--- Early Exercise Analysis ---\n");
    printf("  Early Ex Premium:         $%.6f\n", early_ex_premium);
    
    if (fabs(early_ex_premium) < 0.0001) {
        printf("  → American call ≈ European call (no dividends)\n");
        printf("  → Early exercise is NEVER optimal without dividends!\n");
        printf("  → This validates our implementation\n");
    } else {
        printf("  → WARNING: Unexpected early exercise premium\n");
    }
    
    printf("\n--- LSM vs Binomial ---\n");
    double lsm_error = fabs(lsm_american - binomial_american);
    printf("  Error: $%.4f (%.2f%%)\n", 
           lsm_error, 100.0 * lsm_error / binomial_american);
    
    printf("\n✓ Test completed\n");
    
    mco_context_free(ctx);
}

// ============================================================================
// Test 5: Multiple Scenarios Matrix
// ============================================================================

void test_scenario_matrix() {
    print_header("Test 5: Scenario Matrix - Binomial vs LSM");
    
    MCOContext* ctx = mco_context_new();
    mco_context_set_binomial_steps(ctx, 200);
    mco_context_set_num_simulations(ctx, 50000);
    mco_context_set_antithetic(ctx, 1);
    
    double rate = 0.05;
    double volatility = 0.20;
    double time = 1.0;
    double strike = 100.0;
    
    printf("\nAmerican Put Prices - Varying Spot Prices\n");
    printf("(Strike=$%.0f, r=%.1f%%, σ=%.0f%%, T=%.1f year)\n\n",
           strike, rate*100, volatility*100, time);
    
    printf("Spot  | Moneyness | Binomial | LSM      | Error    | Error %%\n");
    printf("------|-----------|----------|----------|----------|----------\n");
    
    double spots[] = {70.0, 80.0, 90.0, 100.0, 110.0, 120.0, 130.0};
    
    for (int i = 0; i < 7; i++) {
        double spot = spots[i];
        
        double binomial = mco_binomial_american_put(ctx, spot, strike, rate, volatility, time);
        double lsm = mco_lsm_american_put_default(ctx, spot, strike, rate, volatility, time);
        double error = fabs(lsm - binomial);
        double error_pct = 100.0 * error / binomial;
        
        const char* moneyness;
        if (spot < strike * 0.95) moneyness = "Deep ITM";
        else if (spot < strike * 0.98) moneyness = "ITM";
        else if (spot < strike * 1.02) moneyness = "ATM";
        else if (spot < strike * 1.05) moneyness = "OTM";
        else moneyness = "Deep OTM";
        
        printf("$%.0f  | %-9s | $%7.4f | $%7.4f | $%.4f | %.2f%%\n",
               spot, moneyness, binomial, lsm, error, error_pct);
    }
    
    printf("\n✓ Test completed\n");
    
    mco_context_free(ctx);
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  American Option Pricing - Comprehensive Comparison         ║\n");
    printf("║  Binomial Tree vs Least Squares Monte Carlo                 ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    
    test_atm_american_put();
    printf("\n");
    
    test_deep_itm_american_put();
    printf("\n");
    
    test_convergence_analysis();
    printf("\n");
    
    test_american_call();
    printf("\n");
    
    test_scenario_matrix();
    
    printf("\n");
    print_separator();
    printf("  ✓ ALL TESTS COMPLETED SUCCESSFULLY\n");
    print_separator();
    printf("\nKey Takeaways:\n");
    printf("  • Binomial trees provide exact/benchmark prices (deterministic)\n");
    printf("  • LSM provides good approximations (stochastic, may vary)\n");
    printf("  • American puts show significant early exercise value\n");
    printf("  • American calls ≈ European calls (no dividends)\n");
    printf("  • Both methods converge to same values with enough steps/paths\n");
    printf("\n");
    
    return 0;
}
