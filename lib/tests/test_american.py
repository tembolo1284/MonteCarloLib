def test_american_put_early_exercise_premium(ctx):
    """American put should have early exercise premium over European"""
    ffi, mco, context = ctx
    
    # Deep ITM put should have significant early exercise value
    S, K, r, sigma, T = 80.0, 100.0, 0.05, 0.2, 1.0
    
    european = mco.mco_european_put(context, S, K, r, sigma, T)
    american = mco.mco_american_put(context, S, K, r, sigma, T, 50)
    
    # American should be worth more due to early exercise
    assert american > european

def test_american_call_no_dividends(ctx):
    """American call without dividends should equal European"""
    ffi, mco, context = ctx
    mco.mco_context_set_num_simulations(context, 500000)
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.2, 1.0
    
    european = mco.mco_european_call(context, S, K, r, sigma, T)
    american = mco.mco_american_call(context, S, K, r, sigma, T, 50)
    
    # Should be approximately equal (within 5% due to MC)
    assert abs(american - european) / european < 0.05
