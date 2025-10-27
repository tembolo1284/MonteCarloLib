#ifndef MCOPTIONS_AMERICAN_OPTION_HPP
#define MCOPTIONS_AMERICAN_OPTION_HPP

#include "internal/context.hpp"
#include "internal/instruments/instrument.hpp"

namespace mcoptions {

struct AmericanOptionData : OptionData {
    int num_exercise_points;
};

double price_american_option(Context& ctx, const AmericanOptionData& option);

}

#endif
