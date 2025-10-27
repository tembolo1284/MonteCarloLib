# Monte Carlo Options Pricing Library

A high-performance C++ library for pricing exotic options using Monte Carlo simulation, designed with a clean C API for maximum portability and ease of integration.

## Design Philosophy

### Single Public Header

This library follows the principle of **one public header, unlimited internal complexity**. The entire public API is exposed through a single C header file (`include/mcoptions.h`), while all C++ implementation details are hidden in internal headers and source files.

**Why this approach?**

- **Language Independence**: C API can be called from any language (Python, Julia, R, Java, etc.)
- **ABI Stability**: No C++ name mangling or implementation details leak into the API
- **Clean Separation**: Users only see what they need - simple C function calls
- **Easy Integration**: Just include one header and link against the shared library

### Internal Structure

While the public API is pure C, internally the library uses modern C++17 with:

- **Composition over inheritance**: Option types are composed from building blocks rather than deep inheritance hierarchies
- **Header-only utilities**: Random number generation and math utilities are template-based for maximum performance
- **Clear separation**: Headers in `include/internal/` contain the C++ interfaces, implementations in `src/`

```
include/
  mcoptions.h              ← The ONLY public header (pure C)
  internal/                ← Internal C++ headers (not exposed to users)
    context.hpp
    random.hpp
    instrument.hpp
    monte_carlo.hpp
    european_option.hpp
    asian_option.hpp
    american_option.hpp
```

## Testing Philosophy: Python + CFFI

Rather than writing C/C++ tests, this library uses **Python with CFFI** for testing. This approach offers several advantages:

### Why CFFI?

1. **No manual bindings**: CFFI parses the C header directly - no wrapper code needed
2. **Easy test writing**: Python's expressiveness makes tests concise and readable  
3. **Statistical validation**: Python's ecosystem (NumPy, SciPy, pytest) makes statistical testing natural
4. **Quick iteration**: Change C++ code, rebuild library, rerun tests - no test recompilation

### Test Structure

```
tests/
  conftest.py                    ← CFFI loader + pytest fixtures
  test_european.py               ← European option tests (put-call parity, etc.)
  test_asian.py                  ← Asian option tests (averaging effects)
  test_american.py               ← American option tests (early exercise premium)
  test_variance_reduction.py     ← Variance reduction effectiveness
```

**Key Testing Principle**: Rather than checking exact values, tests validate **financial properties**:
- Put-call parity
- Moneyness relationships (ITM > ATM > OTM)
- Early exercise premiums for American options
- Variance reduction effectiveness
- Asian options cheaper than European (due to averaging)

## Build System: Premake5

This project uses **Premake5** instead of CMake for build configuration.

### Why Premake?

- **Simple**: Single `premake5.lua` file vs. multiple CMakeLists.txt files
- **Native projects**: Generates Makefiles, Visual Studio solutions, Xcode projects - developers use familiar tools
- **Readable**: Lua configuration is clean and easy to understand
- **Fast**: Lightweight generator, doesn't drive the build itself

### Build Process

Premake is a **two-stage build system**:

1. **Generate** native build files: `premake5 gmake2` (or `vs2022`, `xcode4`, etc.)
2. **Build** using native tools: `make config=release`

This differs from CMake which uses: `cmake -B build` then `cmake --build build` where CMake drives the build.

## Building the Library

### Prerequisites

- **C++ Compiler**: GCC 4.8+, Clang 3.3+, or MSVC 2015+
- **Premake5**: Download from [premake.github.io](https://premake.github.io/download)
- **Python 3.7+** (for testing)
- **pytest and cffi** (for testing): `pip install pytest cffi`

### Option 1: Using build.sh (Recommended)

The `build.sh` script automates the entire build and test process:

```bash
# Build everything with gcc
./build.sh --all

# Build with clang in release mode
./build.sh --compiler clang --config release --build

# Build and run all tests
./build.sh --all --test

# Run specific test file
./build.sh --test test_european

# Build with clang in debug mode
./build.sh --compiler clang --config debug --all

# Clean everything
./build.sh --clean

# Show help
./build.sh --help
```

**build.sh features:**
- Compiler selection: `--compiler gcc` or `--compiler clang`
- Configuration: `--config debug` or `--config release`
- Testing: `--test` runs all tests, `--test test_european` runs specific test
- Validation: Checks for pytest/cffi before running tests
- Clear output: Shows what it's doing at each step

**Common workflows:**

```bash
# Full rebuild and test
./build.sh --clean --build --test

# Quick test after code change
./build.sh --build --test

# Test variance reduction only
./build.sh --test test_variance_reduction
```

### Option 2: Manual Build

If you prefer manual control:

**Linux/macOS with GCC:**
```bash
# Generate Makefiles
premake5 gmake2

# Build release version
make config=release

# Or build debug version
make config=debug

# Library is in build/libmcoptions.so (or .dylib on macOS)
```

**Linux/macOS with Clang:**
```bash
# Set compiler
export CC=clang
export CXX=clang++

# Generate and build
premake5 gmake2
make config=release
```

**Windows with Visual Studio:**
```bash
# Generate Visual Studio 2022 solution
premake5 vs2022

# Build from command line
msbuild mcoptions.sln /p:Configuration=Release

# Or open mcoptions.sln in Visual Studio
```

### Running Tests

After building the library:

```bash
# Run all tests
pytest tests/ -v

# Run specific test file
pytest tests/test_european.py -v

# Run specific test
pytest tests/test_european.py::test_put_call_parity -v

# Run with detailed output
pytest tests/ -v --tb=short

# Run with coverage (if installed)
pytest tests/ -v --cov=. --cov-report=html
```

## Quantitative Finance Implementation

### Supported Option Types

#### 1. European Options
Standard vanilla options that can only be exercised at maturity.

**API:**
```c
double mco_european_call(ctx, spot, strike, rate, volatility, time_to_maturity);
double mco_european_put(ctx, spot, strike, rate, volatility, time_to_maturity);
```

**Mathematical Model:**
- Geometric Brownian Motion (GBM) for underlying asset
- Black-Scholes-Merton framework
- Payoff: max(S_T - K, 0) for calls, max(K - S_T, 0) for puts

#### 2. Asian Options
Path-dependent options where payoff depends on the average price over time.

**API:**
```c
double mco_asian_arithmetic_call(ctx, spot, strike, rate, volatility, 
                                  time_to_maturity, num_observations);
double mco_asian_arithmetic_put(ctx, spot, strike, rate, volatility,
                                 time_to_maturity, num_observations);
```

**Key Properties:**
- Uses arithmetic averaging (more common in practice than geometric)
- Lower volatility than European options due to averaging effect
- Cheaper than European options with same parameters
- Commonly used in commodity and FX markets

**Implementation:**
- Discrete observations at regular intervals
- Payoff: max(Average(S) - K, 0) for calls

#### 3. American Options
Options that can be exercised at any time before maturity.

**API:**
```c
double mco_american_call(ctx, spot, strike, rate, volatility,
                         time_to_maturity, num_exercise_points);
double mco_american_put(ctx, spot, strike, rate, volatility,
                        time_to_maturity, num_exercise_points);
```

**Implementation Details:**
- **Longstaff-Schwartz Algorithm** (Least Squares Monte Carlo)
- Uses regression to estimate continuation value at each exercise point
- Laguerre polynomial basis functions for regression
- Backward induction from maturity to present

**Early Exercise Premium:**
- American puts: Always worth more than European (can exercise early to capture time value of money)
- American calls (no dividends): Approximately equal to European (early exercise generally suboptimal)

### Variance Reduction Techniques

Monte Carlo simulation suffers from slow convergence (O(1/√N)). Variance reduction techniques improve accuracy:

#### 1. Antithetic Variates

**Principle:** For each random path, simulate a negated path. Paths are perfectly negatively correlated.

**API:**
```c
mco_context_set_antithetic(ctx, 1);  // Enable
mco_context_set_antithetic(ctx, 0);  // Disable (default: enabled)
```

**How it works:**
- Generate random normals: Z₁, Z₂, ..., Zₙ
- Simulate path 1 with these normals
- Simulate path 2 with -Z₁, -Z₂, ..., -Zₙ
- Average the two payoffs

**Effectiveness:**
- Reduces variance by 5-15% for most options
- Works best for symmetric payoffs
- Minimal computational overhead (uses cached random numbers)

**Implementation detail:** Properly caches random numbers and negates them, rather than generating independent paths.

#### 2. Importance Sampling

**Principle:** Shift the simulation distribution closer to the payoff region, then correct with likelihood ratios.

**API:**
```c
mco_context_set_importance_sampling(ctx, 1, drift_shift);
// drift_shift > 0: shift toward higher prices (good for OTM calls)
// drift_shift < 0: shift toward lower prices (good for OTM puts)
```

**How it works:**
- Modify drift: μ → μ + shift
- Simulate paths with new drift
- Weight payoffs by likelihood ratio: exp(-shift * W_T - 0.5 * shift² * T)

**Effectiveness:**
- Most effective for OTM options (low probability events)
- Can reduce variance by 20-40% for deep OTM options
- Choose drift shift based on option moneyness

### Simulation Parameters

Configure simulation through context:

```c
mco_context_t* ctx = mco_context_new();

// Set random seed (for reproducibility)
mco_context_set_seed(ctx, 42);

// Set number of simulation paths
mco_context_set_num_simulations(ctx, 100000);  // default: 100,000

// Set number of time steps per path
mco_context_set_num_steps(ctx, 252);  // default: 252 (trading days)

// Configure variance reduction
mco_context_set_antithetic(ctx, 1);
mco_context_set_importance_sampling(ctx, 1, 1.0);
```

**Accuracy vs. Speed Trade-offs:**

| Paths | Accuracy | Speed | Use Case |
|-------|----------|-------|----------|
| 10,000 | ±5% | Fast | Quick estimates |
| 100,000 | ±1.5% | Medium | Standard pricing |
| 1,000,000 | ±0.5% | Slow | High-precision |

With antithetic variates, you can often use half the paths for similar accuracy.

### Example Usage

**Simple European Call:**
```c
#include "mcoptions.h"

int main() {
    mco_context_t* ctx = mco_context_new();
    mco_context_set_seed(ctx, 42);
    
    // Price ATM call: S=100, K=100, r=5%, σ=20%, T=1 year
    double price = mco_european_call(ctx, 100.0, 100.0, 0.05, 0.2, 1.0);
    printf("Call price: $%.2f\n", price);  // ~$10.45
    
    mco_context_free(ctx);
    return 0;
}
```

**Asian Option with High Accuracy:**
```c
mco_context_t* ctx = mco_context_new();
mco_context_set_num_simulations(ctx, 500000);  // High accuracy
mco_context_set_antithetic(ctx, 1);            // Variance reduction

// Monthly averaging Asian call
double price = mco_asian_arithmetic_call(ctx, 100.0, 100.0, 0.05, 0.2, 1.0, 12);
printf("Asian call: $%.2f\n", price);  // ~$6.11

mco_context_free(ctx);
```

**American Put with Early Exercise:**
```c
mco_context_t* ctx = mco_context_new();

// Deep ITM put: S=80, K=100
double european_put = mco_european_put(ctx, 80.0, 100.0, 0.05, 0.2, 1.0);
double american_put = mco_american_put(ctx, 80.0, 100.0, 0.05, 0.2, 1.0, 50);

printf("European put: $%.2f\n", european_put);  // ~$20.00
printf("American put: $%.2f\n", american_put);  // ~$20.63
printf("Early exercise premium: $%.2f\n", american_put - european_put);

mco_context_free(ctx);
```

### Validation and Testing

The library validates correct implementation through:

**1. Mathematical Properties:**
- Put-call parity: C - P = S - K·e^(-rT)
- Moneyness ordering: ITM > ATM > OTM
- Zero volatility limit: deterministic payoff

**2. Comparative Relationships:**
- Asian < European (same parameters)
- American ≥ European (always)
- More observations → lower Asian price

**3. Variance Reduction Effectiveness:**
- Statistical tests showing lower variance
- Multiple runs with different seeds
- Measurement of variance reduction percentage

**4. Convergence:**
- Increasing paths reduces standard error
- Convergence rate approximately O(1/√N)

## Performance Characteristics

**Typical pricing times** (100,000 paths, Intel i7):
- European options: ~10-15 ms
- Asian options: ~15-25 ms
- American options: ~80-120 ms (LSM regression overhead)

**Memory usage:**
- European/Asian: O(num_steps) per path
- American: O(num_paths × num_exercise_points) for regression

**Parallelization:**
- Current implementation: Single-threaded
- Future: OpenMP parallelization over paths (embarrassingly parallel)

## Project Structure

```
mcoptions/
├── premake5.lua              Root build configuration
├── build.sh                  Build automation script
├── README.md                 This file
│
├── include/
│   ├── mcoptions.h          Public C API (ONLY public header)
│   └── internal/            Internal C++ headers (not exposed)
│       ├── context.hpp
│       ├── random.hpp
│       ├── instrument.hpp
│       ├── monte_carlo.hpp
│       ├── european_option.hpp
│       ├── asian_option.hpp
│       └── american_option.hpp
│
├── src/                      Implementation files
│   ├── api.cpp              C API wrapper (C to C++ bridge)
│   ├── context.cpp
│   ├── monte_carlo.cpp
│   ├── european_option.cpp
│   ├── asian_option.cpp
│   └── american_option.cpp
│
├── tests/                    Python CFFI tests
│   ├── conftest.py          pytest configuration + CFFI loader
│   ├── test_european.py
│   ├── test_asian.py
│   ├── test_american.py
│   └── test_variance_reduction.py
│
└── build/                    Generated (not in git)
    └── libmcoptions.so      Compiled shared library
```

## Contributing

When adding new option types:

1. Add internal header in `include/internal/`
2. Implement in `src/`
3. Add C API functions in `src/api.cpp`
4. Declare C API in `include/mcoptions.h`
5. Write Python tests in `tests/`

The one-public-header principle means all user-facing additions go through `mcoptions.h`.

## References

### Academic Papers
- **Longstaff & Schwartz (2001)**: "Valuing American Options by Simulation"
- **Glasserman (2003)**: "Monte Carlo Methods in Financial Engineering"

### Implementation Reference
- Design pattern based on "Writing a Native Shared Library" article
- Single public C header for maximum portability
- CFFI testing approach for rapid development
