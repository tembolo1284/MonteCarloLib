#ifndef MCOPTIONS_STRATIFIED_SAMPLING_HPP
#define MCOPTIONS_STRATIFIED_SAMPLING_HPP

#include <random>
#include <vector>
#include <cmath>

namespace mcoptions {

// Generate stratified uniform samples
inline std::vector<double> generate_stratified_uniforms(std::mt19937_64& rng, size_t n) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::vector<double> samples(n);
    
    double stratum_size = 1.0 / n;
    
    for (size_t i = 0; i < n; ++i) {
        // Sample uniformly within stratum [i/n, (i+1)/n]
        double u = dist(rng);
        samples[i] = (i + u) * stratum_size;
    }
    
    return samples;
}

// Convert uniform to standard normal using inverse CDF (Box-Muller)
inline double inverse_normal_cdf_simple(double u) {
    // For simplicity, use Box-Muller on pairs
    // In practice, use more sophisticated inverse CDF
    // This is a placeholder - we'll use a better method below
    
    // Clamp to avoid log(0)
    u = std::max(1e-10, std::min(1.0 - 1e-10, u));
    
    // Simple approximation using inverse erf
    double x = 2.0 * u - 1.0;
    double sign = (x < 0.0) ? -1.0 : 1.0;
    x = std::abs(x);
    
    // Abramowitz and Stegun approximation
    double a = 0.147;
    double b = 2.0 / (M_PI * a) + std::log(1.0 - x * x) / 2.0;
    double c = std::log(1.0 - x * x) / a;
    double result = sign * std::sqrt(-b + std::sqrt(b * b - c));
    
    return result * std::sqrt(2.0);
}

// Generate stratified normal samples
inline std::vector<double> generate_stratified_normals(std::mt19937_64& rng, size_t n) {
    auto uniforms = generate_stratified_uniforms(rng, n);
    std::vector<double> normals(n);
    
    for (size_t i = 0; i < n; ++i) {
        normals[i] = inverse_normal_cdf_simple(uniforms[i]);
    }
    
    return normals;
}

} // namespace mcoptions

#endif
