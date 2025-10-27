import pytest

def test_antithetic_variates_reduces_variance(ctx):
    """Test that antithetic variates reduces variance"""
    ffi, mco, context = ctx
    
    prices_without = []
    prices_with = []
    
    for seed in range(10):
        mco.mco_context_set_seed(context, seed)
        mco.mco_context_set_num_simulations(context, 10000)
        
        # Without antithetic variates
        mco.mco_context_set_antithetic(context, 0)
        prices_without.append(mco.mco_european_call(context, 100.0, 100.0, 0.05, 0.2, 1.0))
        
        # With antithetic variates
        mco.mco_context_set_antithetic(context, 1)
        prices_with.append(mco.mco_european_call(context, 100.0, 100.0, 0.05, 0.2, 1.0))
    
    import statistics
    var_without = statistics.variance(prices_without)
    var_with = statistics.variance(prices_with)
    
    # Antithetic variates should reduce variance
    assert var_with < var_without

def test_importance_sampling_effectiveness(ctx):
    """Test that importance sampling reduces variance for OTM options"""
    ffi, mco, context = ctx
    
    prices_without = []
    prices_with = []
    
    for seed in range(10):
        mco.mco_context_set_seed(context, seed)
        mco.mco_context_set_num_simulations(context, 10000)
        
        # Without importance sampling
        mco.mco_context_set_importance_sampling(context, 0, 0.0)
        prices_without.append(mco.mco_european_call(context, 80.0, 100.0, 0.05, 0.2, 1.0))
        
        # With importance sampling (shift drift towards OTM region)
        mco.mco_context_set_importance_sampling(context, 1, 0.1)
        prices_with.append(mco.mco_european_call(context, 80.0, 100.0, 0.05, 0.2, 1.0))
    
    import statistics
    var_without = statistics.variance(prices_without)
    var_with = statistics.variance(prices_with)
    
    # Importance sampling should help for OTM options
    assert var_with < var_without

def test_control_variates_effectiveness(ctx):
    """Control variates should dramatically reduce variance for European options"""
    ffi, mco, context = ctx
    
    prices_without = []
    prices_with = []
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.2, 1.0
    
    for seed in range(10):
        mco.mco_context_set_seed(context, seed)
        mco.mco_context_set_num_simulations(context, 10000)
        mco.mco_context_set_antithetic(context, 0)  # Disable other VR
        
        # Without control variates
        mco.mco_context_set_control_variates(context, 0)
        prices_without.append(mco.mco_european_call(context, S, K, r, sigma, T))
        
        # With control variates
        mco.mco_context_set_control_variates(context, 1)
        prices_with.append(mco.mco_european_call(context, S, K, r, sigma, T))
    
    import statistics
    var_without = statistics.variance(prices_without)
    var_with = statistics.variance(prices_with)
    
    # Control variates should reduce variance by at least 30%
    assert var_with < var_without * 0.7
    
    # Mean should be similar
    mean_without = statistics.mean(prices_without)
    mean_with = statistics.mean(prices_with)
    assert abs(mean_without - mean_with) < 0.5

def test_stratified_sampling_effectiveness(ctx):
    """Stratified sampling should reduce variance"""
    ffi, mco, context = ctx
    
    prices_without = []
    prices_with = []
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.2, 1.0
    
    for seed in range(10):
        mco.mco_context_set_seed(context, seed)
        mco.mco_context_set_num_simulations(context, 10000)
        mco.mco_context_set_antithetic(context, 0)  # Disable other VR
        
        # Without stratified sampling
        mco.mco_context_set_stratified_sampling(context, 0)
        prices_without.append(mco.mco_european_call(context, S, K, r, sigma, T))
        
        # With stratified sampling
        mco.mco_context_set_stratified_sampling(context, 1)
        prices_with.append(mco.mco_european_call(context, S, K, r, sigma, T))
    
    import statistics
    var_without = statistics.variance(prices_without)
    var_with = statistics.variance(prices_with)
    
    # Stratified sampling should reduce variance by at least 10%
    assert var_with < var_without * 0.9
    
    # Mean should be similar
    mean_without = statistics.mean(prices_without)
    mean_with = statistics.mean(prices_with)
    assert abs(mean_without - mean_with) < 0.5

def test_combined_variance_reduction_techniques(ctx):
    """All variance reduction techniques together"""
    ffi, mco, context = ctx
    mco.mco_context_set_num_simulations(context, 50000)
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.2, 1.0
    
    # No variance reduction
    mco.mco_context_set_antithetic(context, 0)
    mco.mco_context_set_control_variates(context, 0)
    mco.mco_context_set_stratified_sampling(context, 0)
    price_none = mco.mco_european_call(context, S, K, r, sigma, T)
    
    # All variance reduction
    mco.mco_context_set_antithetic(context, 1)
    mco.mco_context_set_control_variates(context, 1)
    mco.mco_context_set_stratified_sampling(context, 1)
    price_all = mco.mco_european_call(context, S, K, r, sigma, T)
    
    # Both should give reasonable prices
    assert 7.0 < price_none < 11.0
    assert 7.0 < price_all < 11.0
    
    # Should be close to each other
    assert abs(price_none - price_all) < 1.0

def test_control_variates_accuracy(ctx):
    """Control variates should give more accurate results"""
    ffi, mco, context = ctx
    mco.mco_context_set_num_simulations(context, 100000)
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.2, 1.0
    
    # Black-Scholes analytical price (approximate)
    import math
    d1 = (math.log(S/K) + (r + 0.5*sigma**2)*T) / (sigma * math.sqrt(T))
    d2 = d1 - sigma * math.sqrt(T)
    
    def norm_cdf(x):
        return 0.5 * (1.0 + math.erf(x / math.sqrt(2.0)))
    
    bs_price = S * norm_cdf(d1) - K * math.exp(-r*T) * norm_cdf(d2)
    
    # MC with control variates
    mco.mco_context_set_control_variates(context, 1)
    mc_price = mco.mco_european_call(context, S, K, r, sigma, T)
    
    # Should be very close to analytical
    assert abs(mc_price - bs_price) / bs_price < 0.02

def test_variance_reduction_for_asian(ctx):
    """Variance reduction should work for Asian options too"""
    ffi, mco, context = ctx
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.2, 1.0
    
    # Without variance reduction
    mco.mco_context_set_antithetic(context, 0)
    mco.mco_context_set_num_simulations(context, 10000)
    price_without = mco.mco_asian_arithmetic_call(context, S, K, r, sigma, T, 12)
    
    # With antithetic variates
    mco.mco_context_set_antithetic(context, 1)
    price_with = mco.mco_asian_arithmetic_call(context, S, K, r, sigma, T, 12)
    
    # Both should be reasonable
    assert 4.0 < price_without < 8.0
    assert 4.0 < price_with < 8.0
