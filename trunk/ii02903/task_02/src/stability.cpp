#include "stability.h"

#include <algorithm>
#include <cmath>
#include <sstream>

StabilityResult StabilityAnalyzer::analyze(double a1, double a2) {
    StabilityResult r{};
    const double d = a1 * a1 + 4.0 * a2;  // discriminant of z^2 - a1*z - a2
    if (d >= 0.0) {
        const double s = std::sqrt(d);
        r.re1 = (a1 + s) / 2.0;
        r.re2 = (a1 - s) / 2.0;
        r.im1 = 0.0;
        r.im2 = 0.0;
        r.maxModulus = std::max(std::fabs(r.re1), std::fabs(r.re2));
    } else {
        // complex conjugate pair: both roots have the same modulus
        r.re1 = a1 / 2.0;
        r.re2 = r.re1;
        r.im1 = std::sqrt(-d) / 2.0;
        r.im2 = -r.im1;
        r.maxModulus = std::hypot(r.re1, r.im1);
    }
    r.stable = r.maxModulus < 1.0 - kTolerance;
    return r;
}

bool StabilityAnalyzer::isStableJury(double a1, double a2) {
    return std::fabs(a2) < 1.0 && a1 + a2 < 1.0 && a2 - a1 < 1.0;
}

std::string StabilityAnalyzer::describe(const StabilityResult& r) {
    std::ostringstream os;
    if (r.im1 == 0.0) {
        os << "characteristic roots: z1 = " << r.re1 << ", z2 = " << r.re2;
    } else {
        os << "characteristic roots: z = " << r.re1 << " +/- " << r.im1 << "i";
    }
    os << ", max|z| = " << r.maxModulus;
    return os.str();
}

std::string StabilityAnalyzer::report(bool stable, const std::string& info) {
    if (info.empty()) return "";
    std::string text = "Stability analysis: " + info + "\n";
    if (stable) {
        text += "The system is stable (all |z| < 1).";
    } else {
        text += "!!! WARNING: the system is NOT stable (|z| >= 1) - "
                "the process will diverge or oscillate without decay !!!";
    }
    return text;
}
