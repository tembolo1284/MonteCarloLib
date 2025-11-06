"""
Comprehensive comparison of American option pricing methods:
- Binomial Tree (exact/benchmark)
- Least Squares Monte Carlo (LSM)
- Standard Monte Carlo European (baseline)

This validates both methods and demonstrates their relative strengths.
"""

import pytest
import math


def black_scholes_put(S, K, r, sigma, T):
    """Black-Scholes formula for European put"""
    d1 = (math.log(S / K) + (r + 0.5 * sigma ** 2) * T) / (sigma * math.sqrt(T))
    d2 = d1 - sigma * math.sqrt(T)
    
    from scipy.stats import norm
    return K * math.exp(-r * T) * norm.cdf(-d2) - S * norm.cdf(-d1)


def black_scholes_call(S, K, r, sigma, T):
    """Black-Scholes formula for European call"""
    d1 = (math.log(S / K) + (r + 0.5 * sigma ** 2) * T) / (sigma * math.sqrt(T))
    d2 = d1 - sigma * math.sqrt(T)
    
    from scipy.stats import norm
    return S * norm.cdf(d1) - K * math.exp(-r * T) * norm.cdf(d2)


def test_atm_american_put_comparison(ctx):
    """Compare binomial and LSM for ATM American put"""
    ffi, mco, context = ctx
    
    # Configure methods
    mco.mco_context_set_binomial_steps(context, 200)
    mco.mco_context_set_num_simulations(context, 100000)
    mco.mco_context_set_antithetic(context, 1)
    
    # ATM put
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.20, 1.0
    
    # Price with all three methods
    binomial_american = mco.mco_binomial_american_put(context, S, K, r, sigma, T)
    binomial_european = mco.mco_binomial_european_put(context, S, K, r, sigma, T)
    lsm_american = mco.mco_lsm_american_put_default(context, S, K, r, sigma, T)
    mc_european = mco.mco_european_put(context, S, K, r, sigma, T)
    bs_european = black_scholes_put(S, K, r, sigma, T)
    
    print("\n=== ATM American Put Comparison ===")
    print(f"Black-Scholes (European): ${bs_european:.4f}")
    print(f"MC European:              ${mc_european:.4f}")
    print(f"Binomial European:        ${binomial_european:.4f}")
    print(f"Binomial American:        ${binomial_american:.4f}")
    print(f"LSM American:             ${lsm_american:.4f}")
    
    early_ex_premium = binomial_american - binomial_european
    print(f"\nEarly Exercise Premium:   ${early_ex_premium:.4f}")
    
    # Validate relationships
    assert binomial_american > binomial_european  # Early exercise value
    assert lsm_american > mc_european            # LSM captures early exercise
    
    # LSM should be close to binomial (within 5% for ATM)
    lsm_error = abs(lsm_american - binomial_american)
    lsm_error_pct = 100.0 * lsm_error / binomial_american
    print(f"LSM Error: ${lsm_error:.4f} ({lsm_error_pct:.2f}%)")
    
    assert lsm_error_pct < 5.0  # LSM within 5% of binomial


def test_deep_itm_put_comparison(ctx):
    """Compare methods for deep ITM American put (high early exercise)"""
    ffi, mco, context = ctx
    
    mco.mco_context_set_binomial_steps(context, 200)
    mco.mco_context_set_num_simulations(context, 100000)
    mco.mco_context_set_antithetic(context, 1)
    
    # Deep ITM put - significant early exercise expected
    S, K, r, sigma, T = 80.0, 100.0, 0.05, 0.20, 1.0
    intrinsic = K - S
    
    binomial_american = mco.mco_binomial_american_put(context, S, K, r, sigma, T)
    binomial_european = mco.mco_binomial_european_put(context, S, K, r, sigma, T)
    lsm_american = mco.mco_lsm_american_put_default(context, S, K, r, sigma, T)
    
    print("\n=== Deep ITM American Put ===")
    print(f"Spot: ${S:.2f}, Strike: ${K:.2f}")
    print(f"Intrinsic Value:     ${intrinsic:.4f}")
    print(f"Binomial European:   ${binomial_european:.4f}")
    print(f"Binomial American:   ${binomial_american:.4f}")
    print(f"LSM American:        ${lsm_american:.4f}")
    
    early_ex_premium = binomial_american - binomial_european
    premium_pct = 100.0 * early_ex_premium / intrinsic
    print(f"\nEarly Ex Premium: ${early_ex_premium:.4f} ({premium_pct:.2f}% of intrinsic)")
    
    # Deep ITM should have significant early exercise value
    assert binomial_american > binomial_european
    assert binomial_american >= intrinsic
    
    # LSM should capture most of the early exercise value
    lsm_error = abs(lsm_american - binomial_american)
    print(f"LSM Error: ${lsm_error:.4f}")


def test_american_call_comparison(ctx):
    """Compare methods for American call (should equal European without dividends)"""
    ffi, mco, context = ctx
    
    mco.mco_context_set_binomial_steps(context, 200)
    mco.mco_context_set_num_simulations(context, 100000)
    mco.mco_context_set_antithetic(context, 1)
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.20, 1.0
    
    binomial_american = mco.mco_binomial_american_call(context, S, K, r, sigma, T)
    binomial_european = mco.mco_binomial_european_call(context, S, K, r, sigma, T)
    lsm_american = mco.mco_lsm_american_call_default(context, S, K, r, sigma, T)
    bs_call = black_scholes_call(S, K, r, sigma, T)
    
    print("\n=== American Call (No Dividends) ===")
    print(f"Black-Scholes:     ${bs_call:.4f}")
    print(f"Binomial European: ${binomial_european:.4f}")
    print(f"Binomial American: ${binomial_american:.4f}")
    print(f"LSM American:      ${lsm_american:.4f}")
    
    # American call should equal European (no early exercise without dividends)
    assert abs(binomial_american - binomial_european) < 0.001
    print("✓ American = European (no dividends, as expected)")


def test_convergence_comparison(ctx):
    """Compare convergence behavior of binomial vs LSM"""
    ffi, mco, context = ctx
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.20, 1.0
    
    # Get reference price with high-accuracy binomial
    reference = mco.mco_binomial_american_put_steps(context, S, K, r, sigma, T, 500)
    
    print("\n=== Convergence Comparison ===")
    print(f"Reference Price (Binomial 500 steps): ${reference:.4f}\n")
    
    # Test binomial convergence
    print("Binomial Tree:")
    print("Steps | Price    | Error")
    print("------|----------|--------")
    
    for steps in [50, 100, 200, 300]:
        price = mco.mco_binomial_american_put_steps(context, S, K, r, sigma, T, steps)
        error = abs(price - reference)
        print(f"{steps:5d} | ${price:.4f} | ${error:.4f}")
    
    # Test LSM convergence
    print("\nLeast Squares Monte Carlo:")
    print("Paths  | Price    | Error")
    print("-------|----------|--------")
    
    for paths in [50000, 100000, 200000]:
        mco.mco_context_set_num_simulations(context, paths)
        mco.mco_context_set_antithetic(context, 1)
        price = mco.mco_lsm_american_put_default(context, S, K, r, sigma, T)
        error = abs(price - reference)
        print(f"{paths:6d} | ${price:.4f} | ${error:.4f}")


def test_moneyness_scenarios(ctx):
    """Test both methods across various moneyness levels"""
    ffi, mco, context = ctx
    
    mco.mco_context_set_binomial_steps(context, 200)
    mco.mco_context_set_num_simulations(context, 100000)
    mco.mco_context_set_antithetic(context, 1)
    
    K, r, sigma, T = 100.0, 0.05, 0.20, 1.0
    
    print("\n=== American Put - Varying Moneyness ===")
    print("Spot  | Moneyness | Binomial | LSM      | Error")
    print("------|-----------|----------|----------|--------")
    
    for S in [70.0, 80.0, 90.0, 100.0, 110.0, 120.0]:
        binomial = mco.mco_binomial_american_put(context, S, K, r, sigma, T)
        lsm = mco.mco_lsm_american_put_default(context, S, K, r, sigma, T)
        error = abs(lsm - binomial)
        
        if S < K * 0.90:
            moneyness = "Deep ITM"
        elif S < K * 0.95:
            moneyness = "ITM"
        elif S < K * 1.05:
            moneyness = "ATM"
        elif S < K * 1.10:
            moneyness = "OTM"
        else:
            moneyness = "Deep OTM"
        
        print(f"${S:5.1f} | {moneyness:9s} | ${binomial:7.4f} | ${lsm:7.4f} | ${error:.4f}")


def test_lsm_with_different_exercise_dates(ctx):
    """Test LSM with varying number of exercise dates"""
    ffi, mco, context = ctx
    
    mco.mco_context_set_num_simulations(context, 100000)
    mco.mco_context_set_antithetic(context, 1)
    
    S, K, r, sigma, T = 90.0, 100.0, 0.05, 0.20, 1.0
    
    # Reference with binomial
    binomial = mco.mco_binomial_american_put(context, S, K, r, sigma, T)
    
    print("\n=== LSM - Varying Exercise Dates ===")
    print(f"Binomial Reference: ${binomial:.4f}\n")
    print("Ex Dates | LSM Price | Error")
    print("---------|-----------|--------")
    
    for ex_dates in [10, 25, 50, 100]:
        lsm = mco.mco_lsm_american_put(context, S, K, r, sigma, T, ex_dates)
        error = abs(lsm - binomial)
        print(f"{ex_dates:8d} | ${lsm:8.4f} | ${error:.4f}")


def test_method_speed_comparison(ctx):
    """Compare execution speed of different methods"""
    import time
    
    ffi, mco, context = ctx
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.20, 1.0
    
    print("\n=== Speed Comparison ===")
    
    # Binomial tree
    mco.mco_context_set_binomial_steps(context, 200)
    start = time.time()
    for _ in range(10):
        mco.mco_binomial_american_put(context, S, K, r, sigma, T)
    binomial_time = (time.time() - start) / 10
    
    # LSM
    mco.mco_context_set_num_simulations(context, 50000)
    mco.mco_context_set_antithetic(context, 1)
    start = time.time()
    for _ in range(10):
        mco.mco_lsm_american_put_default(context, S, K, r, sigma, T)
    lsm_time = (time.time() - start) / 10
    
    # Standard MC
    start = time.time()
    for _ in range(10):
        mco.mco_european_put(context, S, K, r, sigma, T)
    mc_time = (time.time() - start) / 10
    
    print(f"Binomial Tree (200 steps): {binomial_time*1000:.1f} ms")
    print(f"LSM (50K paths):           {lsm_time*1000:.1f} ms")
    print(f"Standard MC (50K paths):   {mc_time*1000:.1f} ms")
    print(f"\nSpeedup: Binomial is {lsm_time/binomial_time:.1f}x faster than LSM")


if __name__ == "__main__":
    pytest.main([__file__, "-v", "-s"])
