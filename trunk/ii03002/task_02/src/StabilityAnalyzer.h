#pragma once

#include <array>
#include <algorithm>
#include <complex>
#include <cmath>
#include <limits>
#include <ostream>

// Model1_9 recurrence:
// y[k+1] = a1*y[k] + a2*y[k-1] + a3*y[k-2] + b1*u[k]
// Characteristic polynomial: z^3 - a1*z^2 - a2*z - a3 = 0.
struct StabilityResult {
    std::array<std::complex<double>, 3> roots{};
    double max_modulus{0.0};
    bool is_stable{false};
};

namespace stability_detail {
inline std::array<std::complex<double>, 3> cubicRoots(double a1, double a2, double a3) {
    using Complex = std::complex<double>;
    // Durand-Kerner iteration for the monic cubic. Three distinct initial
    // points on a circle avoid the repeated-root singularity in normal cases.
    const Complex c2{-a1, 0.0};
    const Complex c1{-a2, 0.0};
    const Complex c0{-a3, 0.0};
    auto polynomial = [&](Complex z) { return ((z + c2) * z + c1) * z + c0; };

    const double scale = 1.0 + std::max({std::abs(a1), std::sqrt(std::abs(a2)), std::cbrt(std::abs(a3))});
    const double radius = std::max(1.0, scale);
    const double pi = std::acos(-1.0);
    std::array<Complex, 3> roots{
        std::polar(radius, 0.0),
        std::polar(radius, 2.0 * pi / 3.0),
        std::polar(radius, 4.0 * pi / 3.0)
    };

    for (int iteration = 0; iteration < 2000; ++iteration) {
        double largest_delta = 0.0;
        auto next = roots;
        for (std::size_t i = 0; i < roots.size(); ++i) {
            Complex denominator{1.0, 0.0};
            for (std::size_t j = 0; j < roots.size(); ++j) {
                if (i != j) denominator *= (roots[i] - roots[j]);
            }
            if (std::abs(denominator) < 1e-24) denominator += Complex{1e-12, 1e-12};
            const Complex delta = polynomial(roots[i]) / denominator;
            next[i] = roots[i] - delta;
            largest_delta = std::max(largest_delta, std::abs(delta));
        }
        roots = next;
        if (largest_delta < 1e-13) break;
    }
    return roots;
}
} // namespace stability_detail

inline StabilityResult analyzeStability(double a1, double a2, double a3) {
    StabilityResult result;
    if (!std::isfinite(a1) || !std::isfinite(a2) || !std::isfinite(a3)) {
        result.max_modulus = std::numeric_limits<double>::infinity();
        result.is_stable = false;
        return result;
    }
    result.roots = stability_detail::cubicRoots(a1, a2, a3);
    result.max_modulus = 0.0;
    for (const auto& root : result.roots) {
        result.max_modulus = std::max(result.max_modulus, std::abs(root));
    }
    // Strict inequality: roots on |z|=1 are marginal, not asymptotically stable.
    result.is_stable = result.max_modulus < 1.0 - 1e-10;
    return result;
}

inline bool isStable(double a1, double a2, double a3) {
    return analyzeStability(a1, a2, a3).is_stable;
}

inline void printStabilityReport(double a1, double a2, double a3, std::ostream& out) {
    const auto result = analyzeStability(a1, a2, a3);
    out << "Stability check: z^3 - (" << a1 << ")*z^2 - (" << a2 << ")*z - (" << a3 << ") = 0\n";
    out << "  Roots:";
    for (const auto& root : result.roots) out << " " << root;
    out << "\n  Maximum root modulus: " << result.max_modulus << '\n';
    if (result.is_stable) out << "  System is STABLE (all roots are inside the unit circle).\n";
    else out << "  WARNING: system is UNSTABLE or marginally stable (a root is on/outside the unit circle).\n";
}
