import pytest
import math

def test_european_call_atm(ctx):
    """Test ATM European call"""
    ffi, mco, context = ctx
    
    # ATM option: S=K=100
    price = mco.mco_european_call(context, 100.0, 100.0, 0.05, 0.2, 1.0)
    
    # ATM call should be around 8-10 for these parameters
    assert 7.0 < price < 11.0
    assert price > 0

def test_european_put_atm(ctx):
    """Test ATM European put"""
    ffi, mco, context = ctx
    
    price = mco.mco_european_put(context, 100.0, 100.0, 0.05, 0.2, 1.0)
    
    # ATM put should be around 4-6
    assert 3.0 < price < 7.0
    assert price > 0

def test_european_call_itm(ctx):
    """Test ITM European call"""
    ffi, mco, context = ctx
    
    # Deep ITM: S=120, K=100
    price = mco.mco_european_call(context, 120.0, 100.0, 0.05, 0.2, 1.0)
    
    # Should be at least intrinsic value (20)
    assert price > 20.0

def test_european_put_itm(ctx):
    """Test ITM European put"""
    ffi, mco, context = ctx
    
    # Deep ITM: S=80, K=100
    price = mco.mco_european_put(context, 80.0, 100.0, 0.05, 0.2, 1.0)
    
    # Should be at least intrinsic value (20)
    assert 15.0 < price <  19.0

def test_european_call_otm(ctx):
    """Test OTM European call"""
    ffi, mco, context = ctx
    
    # OTM: S=80, K=100
    price = mco.mco_european_call(context, 80.0, 100.0, 0.05, 0.2, 1.0)
    
    # OTM should be small but positive
    assert 0 < price < 5.0

def test_put_call_parity(ctx):
    """Test put-call parity: C - P = S - K*exp(-rT)"""
    ffi, mco, context = ctx
    mco.mco_context_set_num_simulations(context, 500000)  # High accuracy
    
    S, K, r, sigma, T = 100.0, 100.0, 0.05, 0.2, 1.0
    
    call = mco.mco_european_call(context, S, K, r, sigma, T)
    put = mco.mco_european_put(context, S, K, r, sigma, T)
    
    lhs = call - put
    rhs = S - K * math.exp(-r * T)
    
    # Should be close (within 1% due to MC error)
    assert abs(lhs - rhs) / abs(rhs) < 0.01
