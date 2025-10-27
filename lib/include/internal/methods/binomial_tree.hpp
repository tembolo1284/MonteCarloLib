#ifndef MCOPTIONS_BINOMIAL_TREE_HPP
#define MCOPTIONS_BINOMIAL_TREE_HPP

#include "internal/context.hpp"
#include "internal/instruments/instrument.hpp"

namespace mcoptions {

// TODO: Implement Binomial Tree Method
double price_european_option_tree(Context& ctx, const OptionData& option, int num_steps);

}

#endif
