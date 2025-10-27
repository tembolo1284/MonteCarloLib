#ifndef MCOPTIONS_INSTRUMENT_HPP
#define MCOPTIONS_INSTRUMENT_HPP

#include <algorithm>
#include <cmath>

namespace mcoptions {

enum class OptionType {
    Call,
    Put
};

struct OptionData {
    double spot;
    double strike;
    double rate;
    double volatility;
    double time_to_maturity;
    OptionType type;
};

inline double call_payoff(double spot, double strike) {
    return std::max(0.0, spot - strike);
}

inline double put_payoff(double spot, double strike) {
    return std::max(0.0, strike - spot);
}

inline double payoff(double spot, double strike, OptionType type) {
    return type == OptionType::Call ? call_payoff(spot, strike) : put_payoff(spot, strike);
}

}

#endif
