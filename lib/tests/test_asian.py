def test_asian_call_cheaper_than_european(ctx):
    """Asian options should be cheaper due to averaging"""
    ffi, mco, context = ctx
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.2, 1.0
    
    european = mco.mco_european_call(context, S, K, r, sigma, T)
    asian = mco.mco_asian_arithmetic_call(context, S, K, r, sigma, T, 12)
    
    # Asian should be cheaper due to lower volatility from averaging
    assert asian < european

def test_asian_put_atm(ctx):
    """Test ATM Asian put"""
    ffi, mco, context = ctx
    
    price = mco.mco_asian_arithmetic_put(context, 100.0, 100.0, 0.05, 0.2, 1.0, 12)
    
    assert 2.0 < price < 6.0
    assert price > 0

def test_asian_more_observations_less_variance(ctx):
    """More observations should reduce price (less variance)"""
    ffi, mco, context = ctx
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.2, 1.0
    
    asian_monthly = mco.mco_asian_arithmetic_call(context, S, K, r, sigma, T, 12)
    asian_weekly = mco.mco_asian_arithmetic_call(context, S, K, r, sigma, T, 52)
    
    # More observations = more averaging = lower variance = cheaper
    assert asian_weekly <= asian_monthly
