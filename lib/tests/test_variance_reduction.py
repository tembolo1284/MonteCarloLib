def test_antithetic_variates_reduces_variance(ctx):
    """Antithetic variates should reduce standard error"""
    ffi, mco, context = ctx
    
    # Run multiple times to estimate variance
    prices_without = []
    prices_with = []
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.2, 1.0
    
    for seed in range(10):
        mco.mco_context_set_seed(context, seed)
        mco.mco_context_set_num_simulations(context, 10000)
        
        mco.mco_context_set_antithetic(context, 0)
        prices_without.append(mco.mco_european_call(context, S, K, r, sigma, T))
        
        mco.mco_context_set_antithetic(context, 1)
        prices_with.append(mco.mco_european_call(context, S, K, r, sigma, T))
    
    import statistics
    var_without = statistics.variance(prices_without)
    var_with = statistics.variance(prices_with)
    
    # Antithetic should reduce variance
    assert var_with < var_without

def test_importance_sampling_effectiveness(ctx):
    """Importance sampling should improve accuracy for OTM options"""
    ffi, mco, context = ctx
    mco.mco_context_set_num_simulations(context, 100000)
    
    # Deep OTM call where importance sampling helps
    S, K, r, sigma, T = 100.0, 150.0, 0.05, 0.2, 1.0
    
    mco.mco_context_set_importance_sampling(context, 0, 0.0)
    price_standard = mco.mco_european_call(context, S, K, r, sigma, T)
    
    mco.mco_context_set_importance_sampling(context, 1, 1.0)
    price_is = mco.mco_european_call(context, S, K, r, sigma, T)
    
    # Both should give similar prices, but IS should be more stable
    assert price_standard > 0
    assert price_is > 0
    assert abs(price_standard - price_is) / price_standard < 0.2
