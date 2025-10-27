#include "internal/instruments/barrier_option.hpp"
#include <stdexcept>

namespace mcoptions {

double price_barrier_option(Context& ctx, const BarrierOptionData& option) {
    // TODO: Implement Barrier option pricing
    // Monitor whether spot crosses barrier during simulation
    throw std::runtime_error("Barrier option pricing not yet implemented");
}

}
