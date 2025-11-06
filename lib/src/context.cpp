#include "internal/context.hpp"
#include <random>

namespace mcoptions {

Context::Context()
    : num_simulations_(10000),
      num_steps_(252),
      antithetic_enabled_(false),
      control_variates_enabled_(false),
      stratified_sampling_enabled_(false),
      importance_sampling_enabled_(false),
      drift_shift_(0.0),
      model_(Model::BlackScholes),
      sabr_alpha_(0.0),
      sabr_beta_(1.0),
      sabr_rho_(0.0),
      sabr_nu_(0.0),
      binomial_steps_(100),
      rng_(std::random_device{}())
{}

void Context::set_num_simulations(size_t n) {
    num_simulations_ = n;
}

size_t Context::get_num_simulations() const {
    return num_simulations_;
}

void Context::set_num_steps(size_t n) {
    num_steps_ = n;
}

size_t Context::get_num_steps() const {
    return num_steps_;
}

void Context::set_antithetic(bool enabled) {
    antithetic_enabled_ = enabled;
}

bool Context::get_antithetic() const {
    return antithetic_enabled_;
}

void Context::set_control_variates(bool enabled) {
    control_variates_enabled_ = enabled;
}

bool Context::get_control_variates() const {
    return control_variates_enabled_;
}

void Context::set_stratified_sampling(bool enabled) {
    stratified_sampling_enabled_ = enabled;
}

bool Context::get_stratified_sampling() const {
    return stratified_sampling_enabled_;
}

void Context::set_importance_sampling(bool enabled, double drift_shift) {
    importance_sampling_enabled_ = enabled;
    drift_shift_ = drift_shift;
}

bool Context::get_importance_sampling() const {
    return importance_sampling_enabled_;
}

double Context::get_drift_shift() const {
    return drift_shift_;
}

void Context::set_model(Model model) {
    model_ = model;
}

Context::Model Context::get_model() const {
    return model_;
}

void Context::set_sabr_params(double alpha, double beta, double rho, double nu) {
    sabr_alpha_ = alpha;
    sabr_beta_ = beta;
    sabr_rho_ = rho;
    sabr_nu_ = nu;
}

double Context::get_sabr_alpha() const {
    return sabr_alpha_;
}

double Context::get_sabr_beta() const {
    return sabr_beta_;
}

double Context::get_sabr_rho() const {
    return sabr_rho_;
}

double Context::get_sabr_nu() const {
    return sabr_nu_;
}

void Context::set_binomial_steps(size_t n) {
    binomial_steps_ = n;
}

size_t Context::get_binomial_steps() const {
    return binomial_steps_;
}

std::mt19937_64& Context::get_rng() {
    return rng_;
}

void Context::set_seed(unsigned int seed) {
    rng_.seed(seed);
}

}
