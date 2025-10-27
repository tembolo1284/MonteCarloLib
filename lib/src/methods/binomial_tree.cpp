#include "internal/methods/binomial_tree.hpp"
#include <stdexcept>

namespace mcoptions {

double price_european_option_tree(Context& ctx, const OptionData& option, int num_steps) {
    // TODO: Implement Binomial Tree Method
    // Build recombining tree and work backwards
    throw std::runtime_error("Binomial Tree Method not yet implemented");
}

}
