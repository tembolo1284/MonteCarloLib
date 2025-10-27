#ifndef MCOPTIONS_FINITE_DIFFERENCE_HPP
#define MCOPTIONS_FINITE_DIFFERENCE_HPP

#include "internal/context.hpp"
#include "internal/instruments/instrument.hpp"

namespace mcoptions {

// TODO: Implement Finite Difference Method
double price_european_option_fdm(Context& ctx, const OptionData& option);

}

#endif
