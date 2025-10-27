# Monte Carlo Options Pricing gRPC Server

High-performance options pricing service using Monte Carlo simulation.

## Features

- **Multiple Option Types**: European, American, Asian, Barrier, Lookback, Bermudan
- **Variance Reduction**: Antithetic variates, control variates, stratified sampling
- **Batch Pricing**: Price multiple options in a single request
- **Performance**: Written in C++, optimized Monte Carlo engine
- **gRPC Interface**: Easy to integrate with any language

## Prerequisites
```bash
# Ubuntu/Debian
sudo apt install -y build-essential premake5
sudo apt install -y libgrpc++-dev libprotobuf-dev protobuf-compiler-grpc

# For Python client
pip install grpcio grpcio-tools
```

## Building
```bash
./build.sh
```

## Running

### Start Server
```bash
./build/mcoptions_server
# Server listens on 0.0.0.0:50051
```

### Run C++ Client
```bash
./build/mcoptions_client
```

### Run Python Client
```bash
python3 client/python_client.py
```

## API Examples

### Python Client
```python
import grpc
import mcoptions_pb2
import mcoptions_pb2_grpc

channel = grpc.insecure_channel('localhost:50051')
stub = mcoptions_pb2_grpc.McOptionsServiceStub(channel)

request = mcoptions_pb2.EuropeanRequest(
    spot=100.0,
    strike=100.0,
    rate=0.05,
    volatility=0.2,
    time_to_maturity=1.0,
    config=mcoptions_pb2.SimulationConfig(
        num_simulations=100000,
        antithetic_enabled=True
    )
)

response = stub.PriceEuropeanCall(request)
print(f"Price: ${response.price:.2f}")
```

## Performance

Typical pricing times (100K simulations):
- European: ~10-20ms
- American: ~100-200ms
- Asian: ~15-30ms
- Barrier: ~15-30ms

## Architecture
```
MonteCarloLib/
├── lib/           # Core C++ library
└── server/        # gRPC service
    ├── protos/    # Protocol buffers
    ├── server/    # Server implementation
    ├── client/    # Example clients
    └── generated/ # Auto-generated code
```
