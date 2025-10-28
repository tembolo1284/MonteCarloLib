# Monte Carlo Options Pricing

A high-performance options pricing library and gRPC server built in C++ using Monte Carlo simulation methods.

## Overview

This project provides a complete solution for pricing various types of financial options using Monte Carlo simulation. It consists of two main components:

- **Core Library** (`lib/`) - High-performance C++ pricing engine
- **gRPC Server** (`server/`) - Network-accessible pricing service with clients

## Project Structure
```
MonteCarloLib/
├── lib/                          # Core Options Pricing Library
│   ├── include/                  # Public headers
│   ├── src/                      # Implementation
│   ├── tests/                    # Python tests
│   ├── build.sh                  # Build script
│   └── README.md                 # Library documentation
│
├── server/                       # gRPC Service
│   ├── protos/                   # Protocol buffer definitions
│   ├── server/                   # Server implementation
│   ├── client/                   # C++ and Python clients
│   ├── build.sh                  # Build script
│   └── README.md                 # Server documentation
│
└── README.md                     # This file
```

---

## Core Library (`lib/`)

The core library provides a C API for pricing various option types using Monte Carlo simulation.

### Features

**Option Types:**
- European (Call/Put)
- American (Call/Put) 
- Asian (Arithmetic/Geometric)
- Barrier (Up/Down, In/Out)
- Lookback (Fixed/Floating Strike)
- Bermudan

**Variance Reduction Techniques:**
- Antithetic Variates
- Control Variates
- Stratified Sampling
- Importance Sampling

**Performance:**
- Optimized C++ implementation
- Multi-threaded support (planned)
- Typical pricing: 10-200ms for 100K simulations

### Quick Start
```bash
cd lib
./build.sh --build --test

# Use in your code
#include "mcoptions.h"

auto ctx = mco_context_new();
mco_context_set_num_simulations(ctx, 100000);
double price = mco_european_call(ctx, 100.0, 100.0, 0.05, 0.2, 1.0);
mco_context_free(ctx);
```

[→ Full Library Documentation](lib/README.md)

---

## gRPC Server (`server/`)

A production-ready gRPC service that exposes the pricing library over the network, with built-in clients.

### Features

**Server:**
- gRPC interface for all option types
- Real-time logging with colored output
- Graceful shutdown (Ctrl+C)
- Handles multiple concurrent clients
- Configurable variance reduction

**Clients:**
- **C++ Client** - Interactive menu + CLI support
- **Python Client** - Simple scripting interface

### Quick Start

**Terminal 1 - Start Server:**
```bash
cd server
./build.sh --build --run-server
```

**Terminal 2 - Run Client:**
```bash
# Interactive mode
./build.sh --run-client

# CLI mode
./build/mcoptions_client --european-call --spot 100 --strike 100

# Python
./build.sh --run-python
```

[→ Full Server Documentation](server/README.md)

---

## 🚀 Installation

### Prerequisites

**For Library:**
```bash
sudo apt install -y build-essential premake5 python3 python3-pip
pip3 install pytest cffi
```

**For Server (additional):**
```bash
sudo apt install -y protobuf-compiler libprotobuf-dev \
                    libgrpc++-dev libgrpc-dev protobuf-compiler-grpc
pip3 install grpcio grpcio-tools
```

### Build Everything
```bash
# Build library
cd lib
./build.sh --clean --build --test

# Build server
cd ../server
./build.sh --clean --build
```

---

## Usage Examples

### Example 1: Library (C++)
```cpp
#include "mcoptions.h"

int main() {
    auto ctx = mco_context_new();
    
    // Configure simulation
    mco_context_set_num_simulations(ctx, 100000);
    mco_context_set_antithetic(ctx, 1);
    mco_context_set_control_variates(ctx, 1);
    
    // Price an ATM European call
    double price = mco_european_call(ctx, 100.0, 100.0, 0.05, 0.2, 1.0);
    printf("Price: $%.4f\n", price);
    
    mco_context_free(ctx);
    return 0;
}
```

### Example 2: gRPC Client (Python)
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
    volatility=0.20,
    time_to_maturity=1.0,
    config=mcoptions_pb2.SimulationConfig(
        num_simulations=100000,
        antithetic_enabled=True
    )
)

response = stub.PriceEuropeanCall(request)
print(f"Price: ${response.price:.4f}")
```

### Example 3: Interactive Client
```bash
./build.sh --run-client

# Choose from menu:
#   1. Price European Call
#   2. Price European Put
#   3. Price American Call
#   ...
# Enter parameters with helpful defaults
```

---

## Performance

Typical pricing times on modern hardware (100K simulations):

| Option Type | Time (ms) | Variance Reduction |
|-------------|-----------|-------------------|
| European    | 10-20     | AV + CV           |
| American    | 100-200   | AV                |
| Asian       | 15-30     | AV + CV           |
| Barrier     | 15-30     | AV                |
| Lookback    | 20-40     | AV                |
| Bermudan    | 150-250   | AV                |

*AV = Antithetic Variates, CV = Control Variates*

---

## Testing

### Library Tests
```bash
cd lib
./build.sh --test
# Runs 39 tests covering all option types and variance reduction
```

### Server Integration Test
```bash
# Terminal 1
cd server
./build.sh --run-server

# Terminal 2
./build.sh --run-client
# Or
./build.sh --run-python
```

---

## 📖 Documentation

- [Library Documentation](lib/README.md) - Core API reference
- [Server Documentation](server/README.md) - gRPC service guide
- [API Examples](server/client/) - Client code examples

---

## 🛠️ Architecture
```
┌─────────────────────────────────────────────┐
│           Client Applications               │
│  (C++, Python, Go, Java, etc.)             │
└─────────────────┬───────────────────────────┘
                  │ gRPC
                  ▼
┌─────────────────────────────────────────────┐
│          gRPC Server (server/)              │
│  - Request handling                         │
│  - Logging                                  │
│  - Validation                               │
└─────────────────┬───────────────────────────┘
                  │ C API
                  ▼
┌─────────────────────────────────────────────┐
│      Core Library (lib/)                    │
│  - Monte Carlo simulation                   │
│  - Variance reduction                       │
│  - Option pricing algorithms                │
└─────────────────────────────────────────────┘
```

---

## Academic Reference

If you use this library in academic work, please consider citing:
```bibtex
@software{mcoptions2024,
  title = {Monte Carlo Options Pricing Library},
  author = {Your Name},
  year = {2024},
  url = {https://github.com/yourusername/MonteCarloLib}
}
```

---

## Acknowledgments

- Built with [gRPC](https://grpc.io/)
- Testing with [pytest](https://pytest.org/)
- Build system: [premake5](https://premake.github.io/)

---

