#ifndef STABILITY_ANALYZER_H
#define STABILITY_ANALYZER_H

#include <string>

// Stability conditions are derived from the characteristic roots on the Z-plane.
// Model 1.1: y[k+1] = a*y[k] + b*u[k], characteristic root z=a.
// Model 2.8 is nonlinear and has no global LTI pole criterion; a local test at
// equilibrium y*=0,u*=0 uses derivative dF/dy = a (the delayed-state term vanishes).
// Model 3.6 is a nonlinear Euler discretization. Around y*=0,u*=0 its linearized
// recurrence is y[k+1] = y[k] + dt*b*u[k], with root z=1 (marginal, not asymptotically stable).
class StabilityAnalyzer {
public:
    static bool model11Stable(double a, double epsilon = 1e-12);
    static bool model28LocallyStable(double a, double epsilon = 1e-12);
    static bool model36LocallyAsymptoticallyStable(double dt, double a, double epsilon = 1e-12);
    static std::string model11Message(double a);
};

#endif
