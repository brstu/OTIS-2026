#include "stability_analyzer.h"
#include <cmath>

bool StabilityAnalyzer::model11Stable(double a, double epsilon) {
    return std::isfinite(a) && std::abs(a) < 1.0 - epsilon;
}

bool StabilityAnalyzer::model28LocallyStable(double a, double epsilon) {
    return std::isfinite(a) && std::abs(a) < 1.0 - epsilon;
}

bool StabilityAnalyzer::model36LocallyAsymptoticallyStable(double dt, double a, double epsilon) {
    if (!std::isfinite(dt) || !std::isfinite(a) || dt <= 0.0) return false;
    // Linearization of a*y^3 has zero derivative at y=0: z=1.
    // Therefore it is not asymptotically stable for the zero-input equilibrium.
    (void)epsilon;
    return false;
}

std::string StabilityAnalyzer::model11Message(double a) {
    return model11Stable(a)
        ? "Система Model 11 асимптотически устойчива (|a| < 1)."
        : "ПРЕДУПРЕЖДЕНИЕ: Model 11 не является асимптотически устойчивой (|a| >= 1 или некорректный коэффициент).";
}
