#include "StabilityAnalyzer.h"

#include <cmath>
#include <sstream>

namespace stability {

std::complex<double> firstOrderRoot(double a) {
    return {a, 0.0};
}

std::complex<double> eulerRoot(double a, double dt) {
    return {1.0 - a * dt, 0.0};
}

Class classifyRoot(std::complex<double> z) {
    if (!std::isfinite(z.real()) || !std::isfinite(z.imag())) return Class::Invalid;
    const double r = std::abs(z);
    if (std::abs(r - 1.0) <= kBoundaryEps) return Class::Boundary;
    return r < 1.0 ? Class::Stable : Class::Unstable;
}

Class classifyFirstOrder(double a) {
    return classifyRoot(firstOrderRoot(a));
}

Class classifyEuler(double a, double dt) {
    if (!std::isfinite(a) || !std::isfinite(dt) || dt <= 0.0) return Class::Invalid;
    return classifyRoot(eulerRoot(a, dt));
}

std::string firstOrderWarning(const std::string& tag, double a) {
    const Class c = classifyFirstOrder(a);
    std::ostringstream oss;
    switch (c) {
        case Class::Stable:
            return "";
        case Class::Boundary:
            oss << "[WARN] " << tag << ": |z| = |a| = " << std::abs(a)
                << ", system is on the stability boundary (no decay).";
            break;
        case Class::Unstable:
            oss << "[WARN] " << tag << ": |z| = |a| = " << std::abs(a)
                << " > 1, system is UNSTABLE, output will diverge!";
            break;
        case Class::Invalid:
            oss << "[WARN] " << tag << ": coefficient a must be a finite number.";
            break;
    }
    return oss.str();
}

std::string eulerWarning(const std::string& tag, double a, double dt) {
    const Class c = classifyEuler(a, dt);
    std::ostringstream oss;
    switch (c) {
        case Class::Stable:
            return "";
        case Class::Invalid:
            oss << "[WARN] " << tag << ": a must be finite and dt must be finite and > 0.";
            break;
        case Class::Boundary:
            oss << "[WARN] " << tag << ": |1 - a*dt| = 1, scheme is on the stability boundary.";
            break;
        case Class::Unstable:
            if (a < 0.0) {
                oss << "[WARN] " << tag << ": a < 0, the object itself is unstable (growth), |1 - a*dt| > 1.";
            } else {
                oss << "[WARN] " << tag << ": a*dt > 2, Euler scheme is numerically unstable. Decrease dt.";
            }
            break;
    }
    return oss.str();
}

}  // namespace stability
