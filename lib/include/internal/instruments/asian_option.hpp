#ifndef MCOPTIONS_ASIAN_OPTION_HPP
#define MCOPTIONS_ASIAN_OPTION_HPP

#include "internal/context.hpp"
#include "internal/instruments/instrument.hpp"

namespace mcoptions {

struct AsianOptionData : OptionData {
    size_t num_observations;
};

double price_asian_option(Context& ctx, const AsianOptionData& option);

}

#endif
