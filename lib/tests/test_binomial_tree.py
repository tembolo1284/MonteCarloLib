"""
Test suite for Binomial Tree option pricing

Tests:
- European options vs Black-Scholes
- American options (early exercise)
- Put-Call Parity
- Convergence with increasing steps
- Comparison with Monte Carlo
"""

import pytest
import math


def black_scholes_call(S, K, r, sigma, T):
    """Black-Scholes formula for European call"""
    d1 = (math.log(S / K) + (r + 0.5 * sigma ** 2) * T) / (sigma * math.sqrt(T))
    d2 = d1 - sigma * math.sqrt(T)
    
    from scipy.stats import norm
    return S * norm.cdf(d1) - K * math.exp(-r * T) * norm.cdf(d2)


def black_scholes_put(S, K, r, sigma, T):
    """Black-Scholes formula for European put"""
    d1 = (math.log(S / K) + (r + 0.5 * sigma ** 2) * T) / (sigma * math.sqrt(T))
    d2 = d1 - sigma * math.sqrt(T)
    
    from scipy.stats import norm
    return K * math.exp(-r * T) * norm.cdf(-d2) - S * norm.cdf(-d1)


def test_binomial_context_configuration(ctx):
    """Test binomial steps configuration in context"""
    ffi, mco, context = ctx
    
    # Default should be 100
    default_steps = mco.mco_context_get_binomial_steps(context)
    assert default_steps == 100
    
    # Set and verify
    mco.mco_context_set_binomial_steps(context, 200)
    assert mco.mco_context_get_binomial_steps(context) == 200
    
    mco.mco_context_set_binomial_steps(context, 50)
    assert mco.mco_context_get_binomial_steps(context) == 50


def test_european_call_vs_black_scholes(ctx):
    """Binomial European call should match Black-Scholes"""
    ffi, mco, context = ctx
    mco.mco_context_set_binomial_steps(context, 200)
    
    # ATM call
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.20, 1.0
    
    binomial_price = mco.mco_binomial_european_call(context, S, K, r, sigma, T)
    bs_price = black_scholes_call(S, K, r, sigma, T)
    
    # Should match within 1 cent with 200 steps
    assert abs(binomial_price - bs_price) < 0.01
    print(f"Binomial: ${binomial_price:.4f}, Black-Scholes: ${bs_price:.4f}")


def test_european_put_vs_black_scholes(ctx):
    """Binomial European put should match Black-Scholes"""
    ffi, mco, context = ctx
    mco.mco_context_set_binomial_steps(context, 200)
    
    # ATM put
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.20, 1.0
    
    binomial_price = mco.mco_binomial_european_put(context, S, K, r, sigma, T)
    bs_price = black_scholes_put(S, K, r, sigma, T)
    
    # Should match within 1 cent
    assert abs(binomial_price - bs_price) < 0.01
    print(f"Binomial: ${binomial_price:.4f}, Black-Scholes: ${bs_price:.4f}")


def test_put_call_parity(ctx):
    """Test put-call parity: C - P = S - K*e^(-rT)"""
    ffi, mco, context = ctx
    mco.mco_context_set_binomial_steps(context, 100)
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.20, 1.0
    
    call = mco.mco_binomial_european_call(context, S, K, r, sigma, T)
    put = mco.mco_binomial_european_put(context, S, K, r, sigma, T)
    
    left_side = call - put
    right_side = S - K * math.exp(-r * T)
    
    assert abs(left_side - right_side) < 0.01
    print(f"C - P = ${left_side:.4f}, S - K*e^(-rT) = ${right_side:.4f}")


def test_american_put_early_exercise_value(ctx):
    """American put should be worth more than European (early exercise premium)"""
    ffi, mco, context = ctx
    mco.mco_context_set_binomial_steps(context, 200)
    
    # Deep ITM put
    S, K, r, sigma, T = 80.0, 100.0, 0.05, 0.20, 1.0
    
    american = mco.mco_binomial_american_put(context, S, K, r, sigma, T)
    european = mco.mco_binomial_european_put(context, S, K, r, sigma, T)
    intrinsic = K - S  # Immediate exercise value
    
    # American should be worth more than European
    assert american > european
    
    # American should be at least intrinsic value
    assert american >= intrinsic
    
    early_ex_premium = american - european
    print(f"American: ${american:.4f}, European: ${european:.4f}")
    print(f"Early exercise premium: ${early_ex_premium:.4f}")


def test_american_call_no_dividends(ctx):
    """American call without dividends should equal European call"""
    ffi, mco, context = ctx
    mco.mco_context_set_binomial_steps(context, 200)
    
    # ATM call
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.20, 1.0
    
    american = mco.mco_binomial_american_call(context, S, K, r, sigma, T)
    european = mco.mco_binomial_european_call(context, S, K, r, sigma, T)
    
    # Should be essentially equal (within numerical precision)
    # American call should NOT be exercised early without dividends
    assert abs(american - european) < 0.0001
    print(f"American: ${american:.4f}, European: ${european:.4f}")
    print("✓ No early exercise (as expected without dividends)")


def test_convergence_with_steps(ctx):
    """Binomial should converge to Black-Scholes with increasing steps"""
    ffi, mco, context = ctx
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.20, 1.0
    bs_price = black_scholes_call(S, K, r, sigma, T)
    
    steps_list = [10, 25, 50, 100, 200, 500]
    errors = []
    
    print("\nConvergence analysis:")
    print("Steps | Binomial | Error")
    print("------|----------|--------")
    
    for steps in steps_list:
        binomial = mco.mco_binomial_european_call_steps(context, S, K, r, sigma, T, steps)
        error = abs(binomial - bs_price)
        errors.append(error)
        print(f"{steps:5d} | ${binomial:.4f} | ${error:.4f}")
    
    # Error should decrease with more steps
    assert errors[-1] < errors[0]  # 500 steps better than 10 steps
    assert errors[-1] < 0.01       # 500 steps within 1 cent


def test_explicit_steps_override(ctx):
    """Test explicit step count parameter"""
    ffi, mco, context = ctx
    
    # Set context to 100 steps
    mco.mco_context_set_binomial_steps(context, 100)
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.20, 1.0
    
    # Use context setting (100 steps)
    price_100 = mco.mco_binomial_european_call(context, S, K, r, sigma, T)
    
    # Override with 200 steps
    price_200 = mco.mco_binomial_european_call_steps(context, S, K, r, sigma, T, 200)
    
    # Override with 50 steps
    price_50 = mco.mco_binomial_european_call_steps(context, S, K, r, sigma, T, 50)
    
    # All should be different (different convergence)
    assert price_50 != price_100
    assert price_100 != price_200
    
    print(f"50 steps:  ${price_50:.4f}")
    print(f"100 steps: ${price_100:.4f}")
    print(f"200 steps: ${price_200:.4f}")


def test_binomial_vs_monte_carlo_european(ctx):
    """Compare binomial tree with Monte Carlo for European options"""
    ffi, mco, context = ctx
    
    # Configure both methods
    mco.mco_context_set_binomial_steps(context, 200)
    mco.mco_context_set_num_simulations(context, 100000)
    mco.mco_context_set_antithetic(context, 1)
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.20, 1.0
    
    # Price with both methods
    binomial_call = mco.mco_binomial_european_call(context, S, K, r, sigma, T)
    mc_call = mco.mco_european_call(context, S, K, r, sigma, T)
    bs_call = black_scholes_call(S, K, r, sigma, T)
    
    print(f"\nEuropean Call Comparison:")
    print(f"  Black-Scholes: ${bs_call:.4f}")
    print(f"  Binomial:      ${binomial_call:.4f}")
    print(f"  Monte Carlo:   ${mc_call:.4f}")
    
    # Both should be close to Black-Scholes
    assert abs(binomial_call - bs_call) < 0.01
    assert abs(mc_call - bs_call) < 0.05  # MC has more variance


def test_deep_itm_put(ctx):
    """Test deep in-the-money American put"""
    ffi, mco, context = ctx
    mco.mco_context_set_binomial_steps(context, 200)
    
    # Deep ITM: spot 20% below strike
    S, K, r, sigma, T = 80.0, 100.0, 0.05, 0.20, 1.0
    
    american = mco.mco_binomial_american_put(context, S, K, r, sigma, T)
    european = mco.mco_binomial_european_put(context, S, K, r, sigma, T)
    intrinsic = K - S
    
    # Check relationships
    assert american >= intrinsic  # At least intrinsic
    assert american > european    # Early exercise value
    
    premium = american - european
    premium_pct = 100.0 * premium / european
    
    print(f"\nDeep ITM Put Analysis:")
    print(f"  Intrinsic:     ${intrinsic:.4f}")
    print(f"  European:      ${european:.4f}")
    print(f"  American:      ${american:.4f}")
    print(f"  Early Ex Premium: ${premium:.4f} ({premium_pct:.2f}%)")


def test_deep_otm_put(ctx):
    """Test deep out-of-the-money American put"""
    ffi, mco, context = ctx
    mco.mco_context_set_binomial_steps(context, 200)
    
    # Deep OTM: spot 20% above strike
    S, K, r, sigma, T = 120.0, 100.0, 0.05, 0.20, 1.0
    
    american = mco.mco_binomial_american_put(context, S, K, r, sigma, T)
    european = mco.mco_binomial_european_put(context, S, K, r, sigma, T)
    
    # Both should be very small and approximately equal
    assert american < 1.0
    assert european < 1.0
    assert abs(american - european) < 0.10  # Minimal early exercise value when OTM
    
    print(f"\nDeep OTM Put:")
    print(f"  European: ${european:.4f}")
    print(f"  American: ${american:.4f}")


def test_multiple_maturities(ctx):
    """Test American put across different maturities"""
    ffi, mco, context = ctx
    mco.mco_context_set_binomial_steps(context, 200)
    
    S, K, r, sigma = 90.0, 100.0, 0.05, 0.20
    
    print("\nAmerican Put - Varying Maturity:")
    print("Maturity | American | European | Premium")
    print("---------|----------|----------|--------")
    
    for T in [0.25, 0.5, 1.0, 2.0]:
        american = mco.mco_binomial_american_put(context, S, K, r, sigma, T)
        european = mco.mco_binomial_european_put(context, S, K, r, sigma, T)
        premium = american - european
        
        print(f"{T:8.2f} | ${american:7.4f} | ${european:7.4f} | ${premium:.4f}")
        
        # American should always be >= European
        assert american >= european


if __name__ == "__main__":
    pytest.main([__file__, "-v", "-s"])
