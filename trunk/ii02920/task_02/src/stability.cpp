#include "stability.hpp"
#include <cmath>

Stability classifyPole(double pole) {
    if (!std::isfinite(pole)) return Stability::NonFinite;
    const double radius = std::abs(pole);
    if (radius < 1.0) return Stability::Stable;
    if (radius == 1.0) return Stability::Boundary;
    return Stability::Unstable;
}

std::string poleWarning(double pole) {
    const Stability state = classifyPole(pole);
    if (state == Stability::Stable) return {};
    if (state == Stability::Boundary)
        return "[WARN] |z| = 1. Initial deviations do not decay.";
    if (state == Stability::NonFinite)
        return "[WARN] The pole is not finite. Check the coefficients.";
    return "[WARN] |z| > 1. Nonzero initial deviations grow.";
}
