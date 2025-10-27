#ifndef MCOPTIONS_EUROPEAN_OPTION_HPP
#define MCOPTIONS_EUROPEAN_OPTION_HPP

#include "internal/context.hpp"
#include "internal/instruments/instrument.hpp"

namespace mcoptions {

double price_european_option(Context& ctx, const OptionData& option);

}

#endif
