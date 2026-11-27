#include "StabilityAnalyzer.h"

#include <cmath>
#include <sstream>

StabilityResult StabilityAnalyzer::analyzeFirstOrder(double a, double tolerance)
{
    StabilityResult result;
    if (!std::isfinite(a) || !std::isfinite(tolerance) || tolerance < 0.0) {
        result.message = "Invalid coefficient or tolerance for stability analysis.";
        return result;
    }

    const std::complex<double> root(a, 0.0);
    result.roots.push_back(root);
    const double modulus = std::abs(root);
    result.asymptoticallyStable = modulus < 1.0 - tolerance;
    result.marginallyStable = std::abs(modulus - 1.0) <= tolerance;

    std::ostringstream out;
    out << "Characteristic equation: z - " << a << " = 0; root z = " << root
        << "; |z| = " << modulus << ". ";
    if (result.asymptoticallyStable) {
        out << "The system is asymptotically stable (|z| < 1).";
    } else if (result.marginallyStable) {
        out << "The system is on the stability boundary (|z| = 1); it is not asymptotically stable.";
    } else {
        out << "The system is unstable (|z| > 1).";
    }
    result.message = out.str();
    return result;
}
