import pytest

def test_bermudan_call_two_dates(ctx):
    """Test Bermudan call with two exercise dates"""
    ffi, mco, context = ctx
    
    # Exercise dates at 0.5 and 1.0 years
    exercise_dates = ffi.new("double[]", [0.5, 1.0])
    price = mco.mco_bermudan_call(context, 100.0, 100.0, 0.05, 0.2, exercise_dates, 2)
    
    # Should be between European and American
    european = mco.mco_european_call(context, 100.0, 100.0, 0.05, 0.2, 1.0)
    american = mco.mco_american_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 50)
    
    assert european * 0.95 <= price <= american * 1.05

def test_bermudan_call_quarterly(ctx):
    """Test Bermudan call with quarterly exercise dates"""
    ffi, mco, context = ctx
    
    # Quarterly exercise: 0.25, 0.5, 0.75, 1.0 years
    exercise_dates = ffi.new("double[]", [0.25, 0.5, 0.75, 1.0])
    price = mco.mco_bermudan_call(context, 100.0, 100.0, 0.05, 0.2, exercise_dates, 4)
    
    # With more exercise dates, should be closer to American
    american = mco.mco_american_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 50)
    
    assert price > 0
    # Should be close to American with many exercise dates
    assert abs(price - american) / american < 0.1

def test_bermudan_put_itm(ctx):
    """Test ITM Bermudan put"""
    ffi, mco, context = ctx
    
    # Deep ITM put with early exercise opportunities
    exercise_dates = ffi.new("double[]", [0.25, 0.5, 0.75, 1.0])
    
    # Note: We only have bermudan_call in the API, so we test what we have
    # In a complete implementation, you'd add bermudan_put
    price = mco.mco_bermudan_call(context, 120.0, 100.0, 0.05, 0.2, exercise_dates, 4)
    
    # ITM call should have high value
    assert price > 20.0

def test_bermudan_monthly_exercise(ctx):
    """Test Bermudan with monthly exercise dates"""
    ffi, mco, context = ctx
    
    # Monthly exercise over 1 year
    monthly_dates = [i/12.0 for i in range(1, 13)]
    exercise_dates = ffi.new("double[]", monthly_dates)
    price = mco.mco_bermudan_call(context, 100.0, 100.0, 0.05, 0.2, exercise_dates, 12)
    
    # With 12 exercise dates, should be very close to American
    american = mco.mco_american_call(context, 100.0, 100.0, 0.05, 0.2, 1.0, 50)
    
    assert abs(price - american) / american < 0.05

def test_bermudan_single_date_equals_european(ctx):
    """Bermudan with single exercise date should equal European"""
    ffi, mco, context = ctx
    mco.mco_context_set_num_simulations(context, 100000)
    
    # Single exercise date at maturity
    exercise_dates = ffi.new("double[]", [1.0])
    bermudan = mco.mco_bermudan_call(context, 100.0, 100.0, 0.05, 0.2, exercise_dates, 1)
    european = mco.mco_european_call(context, 100.0, 100.0, 0.05, 0.2, 1.0)
    
    # Should be approximately equal
    assert abs(bermudan - european) / european < 0.05

def test_bermudan_atm(ctx):
    """Test ATM Bermudan call"""
    ffi, mco, context = ctx
    
    exercise_dates = ffi.new("double[]", [0.5, 1.0])
    price = mco.mco_bermudan_call(context, 100.0, 100.0, 0.05, 0.2, exercise_dates, 2)
    
    # Reasonable range for ATM call
    assert 7.0 < price < 11.0
