#include "internal/instruments/bermudan_option.hpp"
#include <stdexcept>

namespace mcoptions {

double price_bermudan_option(Context& ctx, const BermudanOptionData& option) {
    // TODO: Implement Bermudan option pricing using LSM
    // Similar to American but only exercise on specific dates
    throw std::runtime_error("Bermudan option pricing not yet implemented");
}

}
