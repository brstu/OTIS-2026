#pragma once

#include "models.hpp"
#include <array>
#include <complex>

struct StabilityResult {
    std::array<std::complex<double>, 3> roots;
    bool stable;
};

StabilityResult analyzeStability(const LinearParameters& parameters,
                                 double tolerance = 1e-9);
