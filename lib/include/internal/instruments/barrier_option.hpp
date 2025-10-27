#ifndef MCOPTIONS_BARRIER_OPTION_HPP
#define MCOPTIONS_BARRIER_OPTION_HPP

#include "internal/context.hpp"
#include "internal/instruments/instrument.hpp"

namespace mcoptions {

enum class BarrierType {
    UpAndOut,
    UpAndIn,
    DownAndOut,
    DownAndIn
};

struct BarrierOptionData {
    double spot;
    double strike;
    double rate;
    double volatility;
    double time_to_maturity;
    OptionType type;
    double barrier_level;
    BarrierType barrier_type;
    double rebate;
};

// TODO: Implement Barrier option pricing
double price_barrier_option(Context& ctx, const BarrierOptionData& option);

}

#endif
