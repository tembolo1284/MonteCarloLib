#include "internal/methods/finite_difference.hpp"
#include <stdexcept>

namespace mcoptions {

double price_european_option_fdm(Context& ctx, const OptionData& option) {
    // TODO: Implement Finite Difference Method
    // Solve Black-Scholes PDE on grid
    throw std::runtime_error("Finite Difference Method not yet implemented");
}

}
