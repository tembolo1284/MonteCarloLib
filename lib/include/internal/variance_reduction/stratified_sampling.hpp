#ifndef MCOPTIONS_STRATIFIED_SAMPLING_HPP
#define MCOPTIONS_STRATIFIED_SAMPLING_HPP

#include <random>
#include <vector>
#include <cmath>
#include <algorithm>

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
    
    // Shuffle to remove ordering bias
    std::shuffle(samples.begin(), samples.end(), rng);
    
    return samples;
}

// Peter Acklam's inverse normal CDF approximation
inline double inverse_normal_cdf(double p) {
    static const double a[6] = {
        -3.969683028665376e+01,
         2.209460984245205e+02,
        -2.759285104469687e+02,
         1.383577518672690e+02,
        -3.066479806614716e+01,
         2.506628277459239e+00
    };
    
    static const double b[5] = {
        -5.447609879822406e+01,
         1.615858368580409e+02,
        -1.556989798598866e+02,
         6.680131188771972e+01,
        -1.328068155288572e+01
    };
    
    static const double c[6] = {
        -7.784894002430283e-03,
        -3.223964580411365e-01,
        -2.400758277161838e+00,
        -2.549732539343734e+00,
         4.374664141464968e+00,
         2.938163982698783e+00
    };
    
    static const double d[4] = {
         7.784695709041462e-03,
         3.224671290700398e-01,
         2.445134137142996e+00,
         3.754408661907416e+00
    };
    
    static const double p_low  = 0.02425;
    static const double p_high = 1.0 - p_low;
    
    double q, r, x;
    
    if (p < 0.0 || p > 1.0) {
        return NAN;
    } else if (p == 0.0) {
        return -INFINITY;
    } else if (p == 1.0) {
        return INFINITY;
    } else if (p < p_low) {
        q = std::sqrt(-2.0 * std::log(p));
        x = (((((c[0]*q + c[1])*q + c[2])*q + c[3])*q + c[4])*q + c[5]) /
            ((((d[0]*q + d[1])*q + d[2])*q + d[3])*q + 1.0);
    } else if (p <= p_high) {
        q = p - 0.5;
        r = q * q;
        x = (((((a[0]*r + a[1])*r + a[2])*r + a[3])*r + a[4])*r + a[5]) * q /
            (((((b[0]*r + b[1])*r + b[2])*r + b[3])*r + b[4])*r + 1.0);
    } else {
        q = std::sqrt(-2.0 * std::log(1.0 - p));
        x = -(((((c[0]*q + c[1])*q + c[2])*q + c[3])*q + c[4])*q + c[5]) /
             ((((d[0]*q + d[1])*q + d[2])*q + d[3])*q + 1.0);
    }
    
    return x;
}

// Generate stratified normal samples - USE WITH CAUTION FOR MULTI-STEP PATHS
// Stratifying every time step can introduce bias in path-dependent simulations
// Better for single-period or terminal-value-only simulations
inline std::vector<double> generate_stratified_normals(std::mt19937_64& rng, size_t n) {
    auto uniforms = generate_stratified_uniforms(rng, n);
    std::vector<double> normals(n);
    
    for (size_t i = 0; i < n; ++i) {
        normals[i] = inverse_normal_cdf(uniforms[i]);
    }
    
    return normals;
}

} // namespace mcoptions

#endif
