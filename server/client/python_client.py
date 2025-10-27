#!/usr/bin/env python3
"""
Python client for Monte Carlo Options Pricing Service
"""

import grpc
import sys
sys.path.insert(0, '../build')

import mcoptions_pb2
import mcoptions_pb2_grpc


class McOptionsClient:
    def __init__(self, server_address='localhost:50051'):
        self.channel = grpc.insecure_channel(server_address)
        self.stub = mcoptions_pb2_grpc.McOptionsServiceStub(self.channel)
    
    def price_european_call(self, spot, strike, rate, volatility, time_to_maturity):
        """Price a European call option"""
        config = mcoptions_pb2.SimulationConfig(
            num_simulations=100000,
            num_steps=252,
            antithetic_enabled=True,
            control_variates_enabled=True
        )
        
        request = mcoptions_pb2.EuropeanRequest(
            spot=spot,
            strike=strike,
            rate=rate,
            volatility=volatility,
            time_to_maturity=time_to_maturity,
            config=config
        )
        
        response = self.stub.PriceEuropeanCall(request)
        return response.price, response.computation_time_ms
    
    def price_american_put(self, spot, strike, rate, volatility, time_to_maturity):
        """Price an American put option"""
        config = mcoptions_pb2.SimulationConfig(
            num_simulations=50000,
            num_steps=252
        )
        
        request = mcoptions_pb2.AmericanRequest(
            spot=spot,
            strike=strike,
            rate=rate,
            volatility=volatility,
            time_to_maturity=time_to_maturity,
            num_exercise_points=50,
            config=config
        )
        
        response = self.stub.PriceAmericanPut(request)
        return response.price, response.computation_time_ms
    
    def price_asian_call(self, spot, strike, rate, volatility, time_to_maturity, num_obs=12):
        """Price an Asian call option"""
        config = mcoptions_pb2.SimulationConfig(
            num_simulations=100000,
            num_steps=252,
            antithetic_enabled=True
        )
        
        request = mcoptions_pb2.AsianRequest(
            spot=spot,
            strike=strike,
            rate=rate,
            volatility=volatility,
            time_to_maturity=time_to_maturity,
            num_observations=num_obs,
            config=config
        )
        
        response = self.stub.PriceAsianCall(request)
        return response.price, response.computation_time_ms


def main():
    print("=" * 50)
    print("  Monte Carlo Options Pricing - Python Client")
    print("=" * 50)
    print()
    
    client = McOptionsClient()
    
    # ATM European Call
    print("Pricing ATM European Call (S=100, K=100, r=5%, σ=20%, T=1y):")
    price, time_ms = client.price_european_call(100.0, 100.0, 0.05, 0.2, 1.0)
    print(f"  Price: ${price:.4f} (computed in {time_ms}ms)")
    print()
    
    # ITM American Put
    print("Pricing ITM American Put (S=80, K=100, r=5%, σ=20%, T=1y):")
    price, time_ms = client.price_american_put(80.0, 100.0, 0.05, 0.2, 1.0)
    print(f"  Price: ${price:.4f} (computed in {time_ms}ms)")
    print()
    
    # Asian Call
    print("Pricing Asian Call with monthly observations:")
    price, time_ms = client.price_asian_call(100.0, 100.0, 0.05, 0.2, 1.0, 12)
    print(f"  Price: ${price:.4f} (computed in {time_ms}ms)")
    print()
    
    print("=" * 50)


if __name__ == '__main__':
    main()
