#ifndef MCOPTIONS_RANDOM_HPP
#define MCOPTIONS_RANDOM_HPP

#include <random>
#include <cmath>
#include <vector>

namespace mcoptions {

inline double box_muller(std::mt19937_64& rng) {
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    double u1 = uniform(rng);
    double u2 = uniform(rng);
    return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
}

inline std::vector<double> generate_normal_samples(std::mt19937_64& rng, size_t n) {
    std::vector<double> samples(n);
    for (size_t i = 0; i < n; ++i) {
        samples[i] = box_muller(rng);
    }
    return samples;
}

}

#endif
