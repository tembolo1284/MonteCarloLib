import pytest

def test_lookback_fixed_strike_call(ctx):
    """Test fixed strike lookback call"""
    ffi, mco, context = ctx
    
    price = mco.mco_lookback_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 1)
    
    # Should be more expensive than vanilla (you get the max price)
    vanilla = mco.mco_european_call(context, 100.0, 100.0, 0.05, 0.2, 1.0)
    assert price > vanilla

def test_lookback_floating_strike_call(ctx):
    """Test floating strike lookback call"""
    ffi, mco, context = ctx
    
    price = mco.mco_lookback_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 0)
    
    # Always positive value (S_T - S_min always >= 0)
    assert price > 0
    
    # Should be valuable since you always get a positive payoff
    assert price > 5.0

def test_lookback_fixed_vs_floating(ctx):
    """Fixed and floating lookback should have different values"""
    ffi, mco, context = ctx
    
    fixed = mco.mco_lookback_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 1)
    floating = mco.mco_lookback_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 0)
    
    # Should be different (typically floating is more expensive)
    assert abs(fixed - floating) > 1.0

def test_lookback_fixed_strike_itm(ctx):
    """Test ITM fixed strike lookback"""
    ffi, mco, context = ctx
    
    # Strike below spot
    price = mco.mco_lookback_call(context, 100.0, 80.0, 0.05, 0.2, 1.0, 1)
    
    # Should be very valuable
    assert price > 20.0

def test_lookback_fixed_strike_otm(ctx):
    """Test OTM fixed strike lookback"""
    ffi, mco, context = ctx
    
    # Strike above spot
    price = mco.mco_lookback_call(context, 100.0, 120.0, 0.05, 0.2, 1.0, 1)
    
    # Still has value due to lookback feature
    assert price > 0

def test_lookback_floating_always_positive(ctx):
    """Floating strike lookback always has positive value"""
    ffi, mco, context = ctx
    
    # Various scenarios
    scenarios = [
        (80.0, 100.0),   # ITM
        (100.0, 100.0),  # ATM
        (120.0, 100.0),  # OTM
    ]
    
    for spot, strike in scenarios:
        price = mco.mco_lookback_call(context, spot, strike, 0.05, 0.2, 1.0, 0)
        # Always positive since payoff = S_T - S_min
        assert price > 0

def test_lookback_high_volatility_impact(ctx):
    """Higher volatility should increase lookback value more than vanilla"""
    ffi, mco, context = ctx
    
    S, K, r, T = 100.0, 100.0, 0.05, 1.0
    
    # Low volatility
    lookback_low = mco.mco_lookback_call(context, S, K, r, 0.1, T, 1)
    vanilla_low = mco.mco_european_call(context, S, K, r, 0.1, T)
    
    # High volatility
    lookback_high = mco.mco_lookback_call(context, S, K, r, 0.3, T, 1)
    vanilla_high = mco.mco_european_call(context, S, K, r, 0.3, T)
    
    # Lookback should benefit more from higher volatility
    lookback_increase = lookback_high - lookback_low
    vanilla_increase = vanilla_high - vanilla_low
    
    assert lookback_increase > vanilla_increase

def test_lookback_fixed_strike_put(ctx):
    """Test fixed strike lookback put"""
    ffi, mco, context = ctx
    
    price = mco.mco_lookback_put(context, 100.0, 100.0, 0.05, 0.2, 1.0, 1)
    vanilla = mco.mco_european_put(context, 100.0, 100.0, 0.05, 0.2, 1.0)
    
    assert price > vanilla

def test_lookback_floating_strike_put(ctx):
    """Test floating strike lookback put"""
    ffi, mco, context = ctx
    
    price = mco.mco_lookback_put(context, 100.0, 100.0, 0.05, 0.2, 1.0, 0)
    
    assert price > 0
    assert price > 5.0
