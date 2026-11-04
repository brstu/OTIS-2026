#include "stability.hpp"
#include <algorithm>
#include <cmath>

namespace {
std::complex<double> polynomial(std::complex<double> z,
                                const LinearParameters& p) {
    return z*z*z - p.a1*z*z - p.a2*z - p.a3;
}
}

StabilityResult analyzeStability(const LinearParameters& p, double tolerance) {
    // Durand-Kerner: simultaneously refines all three roots of
    // z^3-a1*z^2-a2*z-a3=0. The starting points must be different.
    std::array<std::complex<double>, 3> roots = {
        std::complex<double>(1.0, 0.0),
        std::complex<double>(-0.4, 0.9),
        std::complex<double>(-0.4, -0.9)
    };
    for (int iteration = 0; iteration < 200; ++iteration) {
        double largestChange = 0.0;
        auto old = roots;
        for (std::size_t i = 0; i < roots.size(); ++i) {
            std::complex<double> denominator(1.0, 0.0);
            for (std::size_t j = 0; j < roots.size(); ++j)
                if (i != j) denominator *= (old[i] - old[j]);
            roots[i] = old[i] - polynomial(old[i], p) / denominator;
            largestChange = std::max(largestChange, std::abs(roots[i] - old[i]));
        }
        if (largestChange < 1e-13) break;
    }
    bool stable = true;
    for (const auto& root : roots)
        if (std::abs(root) >= 1.0 - tolerance) stable = false;
    return {roots, stable};
}
