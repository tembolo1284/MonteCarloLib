import pytest

def test_barrier_up_and_out_call(ctx):
    """Test up-and-out barrier call"""
    ffi, mco, context = ctx
    
    # Barrier above spot
    price = mco.mco_barrier_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 120.0, 0, 0.0)
    
    # Should be cheaper than vanilla
    vanilla = mco.mco_european_call(context, 100.0, 100.0, 0.05, 0.2, 1.0)
    assert price < vanilla
    assert price > 0

def test_barrier_up_and_in_call(ctx):
    """Test up-and-in barrier call"""
    ffi, mco, context = ctx
    
    # Barrier above spot
    up_in = mco.mco_barrier_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 120.0, 1, 0.0)
    up_out = mco.mco_barrier_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 120.0, 0, 0.0)
    vanilla = mco.mco_european_call(context, 100.0, 100.0, 0.05, 0.2, 1.0)
    
    # up-and-in + up-and-out should approximately equal vanilla
    assert abs((up_in + up_out) - vanilla) / vanilla < 0.15

def test_barrier_down_and_out_call(ctx):
    """Test down-and-out barrier call"""
    ffi, mco, context = ctx
    
    # Barrier below spot
    price = mco.mco_barrier_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 80.0, 2, 0.0)
    
    # Should be cheaper than vanilla
    vanilla = mco.mco_european_call(context, 100.0, 100.0, 0.05, 0.2, 1.0)
    assert price < vanilla
    assert price > 0

def test_barrier_down_and_in_call(ctx):
    """Test down-and-in barrier call"""
    ffi, mco, context = ctx
    
    # Barrier below spot
    down_in = mco.mco_barrier_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 80.0, 3, 0.0)
    down_out = mco.mco_barrier_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 80.0, 2, 0.0)
    vanilla = mco.mco_european_call(context, 100.0, 100.0, 0.05, 0.2, 1.0)
    
    # down-and-in + down-and-out should approximately equal vanilla
    assert abs((down_in + down_out) - vanilla) / vanilla < 0.15

def test_barrier_with_rebate(ctx):
    """Test barrier with rebate payment"""
    ffi, mco, context = ctx
    
    # Up-and-out with rebate
    without_rebate = mco.mco_barrier_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 120.0, 0, 0.0)
    with_rebate = mco.mco_barrier_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 120.0, 0, 5.0)
    
    # With rebate should be worth more
    assert with_rebate > without_rebate

def test_barrier_far_away_equals_vanilla(ctx):
    """Barrier far from spot should equal vanilla"""
    ffi, mco, context = ctx
    mco.mco_context_set_num_simulations(context, 100000)
    
    # Barrier very far above spot (unlikely to hit)
    barrier_far = mco.mco_barrier_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 200.0, 0, 0.0)
    vanilla = mco.mco_european_call(context, 100.0, 100.0, 0.05, 0.2, 1.0)
    
    # Should be approximately equal
    assert abs(barrier_far - vanilla) / vanilla < 0.05

def test_barrier_at_spot_worthless(ctx):
    """Barrier at spot (up-and-out) should be nearly worthless"""
    ffi, mco, context = ctx
    
    # Barrier at current spot - will knock out immediately
    price = mco.mco_barrier_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 100.0, 0, 0.0)
    
    # Should be very small (almost certainly knocked out)
    assert price < 1.0

def test_barrier_itm_vs_otm(ctx):
    """ITM barrier should be worth more than OTM barrier"""
    ffi, mco, context = ctx
    
    # ITM call with barrier
    itm = mco.mco_barrier_call(context, 120.0, 100.0, 0.05, 0.2, 1.0, 150.0, 0, 0.0)
    
    # OTM call with barrier
    otm = mco.mco_barrier_call(context, 80.0, 100.0, 0.05, 0.2, 1.0, 120.0, 0, 0.0)
    
    assert itm > otm
