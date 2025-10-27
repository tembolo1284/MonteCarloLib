#ifndef MCOPTIONS_ASIAN_OPTION_HPP
#define MCOPTIONS_ASIAN_OPTION_HPP

#include "context.hpp"
#include "instrument.hpp"

namespace mcoptions {

struct AsianOptionData : OptionData {
    int num_observations;
};

double price_asian_option(Context& ctx, const AsianOptionData& option);

}

#endif
