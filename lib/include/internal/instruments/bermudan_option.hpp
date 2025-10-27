#ifndef MCOPTIONS_BERMUDAN_OPTION_HPP
#define MCOPTIONS_BERMUDAN_OPTION_HPP

#include "internal/context.hpp"
#include "internal/instruments/instrument.hpp"
#include <vector>

namespace mcoptions {

struct BermudanOptionData {
    double spot;
    double strike;
    double rate;
    double volatility;
    double time_to_maturity;
    OptionType type;
    std::vector<double> exercise_dates;
};

// TODO: Implement Bermudan option pricing
double price_bermudan_option(Context& ctx, const BermudanOptionData& option);

}

#endif
