#ifndef MCOPTIONS_LOOKBACK_OPTION_HPP
#define MCOPTIONS_LOOKBACK_OPTION_HPP

#include "internal/context.hpp"
#include "internal/instruments/instrument.hpp"

namespace mcoptions {

struct LookbackOptionData {
    double spot;
    double strike;
    double rate;
    double volatility;
    double time_to_maturity;
    OptionType type;
    bool fixed_strike;  // true = fixed strike, false = floating strike
};

double price_lookback_option(Context& ctx, const LookbackOptionData& option);

}

#endif
