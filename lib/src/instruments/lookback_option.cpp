#include "internal/instruments/lookback_option.hpp"
#include <stdexcept>

namespace mcoptions {

double price_lookback_option(Context& ctx, const LookbackOptionData& option) {
    // TODO: Implement Lookback option pricing
    // Track min/max spot along path
    throw std::runtime_error("Lookback option pricing not yet implemented");
}

}
