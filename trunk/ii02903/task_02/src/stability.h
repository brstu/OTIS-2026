#pragma once
// Stability analysis of the linear model 1.3:
//   y(t+1) = a1*y(t) + a2*y(t-1) + b*u(t)
// Characteristic equation on the Z-plane:  z^2 - a1*z - a2 = 0.
// The process is stable if and only if both roots lie strictly inside the unit circle (|z| < 1).
#include <string>

// Roots z1 = re1 + i*im1, z2 = re2 + i*im2 and the verdict.
struct StabilityResult {
    double re1;
    double im1;
    double re2;
    double im2;
    double maxModulus;  // max(|z1|, |z2|)
    bool stable;        // true only if maxModulus < 1
};

class StabilityAnalyzer {
public:
    // A root on the unit circle (|z| = 1) is NOT stable (the process does not decay).
    // Because of rounding errors the comparison is made with this small margin.
    static constexpr double kTolerance = 1e-9;

    // Finds the roots of z^2 - a1*z - a2 = 0 and checks |z| < 1.
    static StabilityResult analyze(double a1, double a2);

    // The same verdict without the roots, using the Jury criterion
    // (|a2| < 1, a1 + a2 < 1, a2 - a1 < 1). Used as an independent check.
    static bool isStableJury(double a1, double a2);

    // "characteristic roots: z1 = ..., z2 = ..., max|z| = ..."
    static std::string describe(const StabilityResult& r);

    // Text printed before the simulation. 'info' is the description of the roots.
    // For an unstable system the text ends with a warning, for an empty 'info' the result is empty.
    static std::string report(bool stable, const std::string& info);
};
