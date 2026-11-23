#pragma once

#include <complex>
#include <string>
#include <vector>

// Анализ устойчивости линейной дискретной модели первого порядка:
// y[k+1] = a*y[k] + входная часть. Характеристическое уравнение z-a=0.
struct StabilityResult {
    std::vector<std::complex<double>> roots;
    bool asymptoticallyStable = false;
    bool marginallyStable = false;
    std::string message;
};

class StabilityAnalyzer {
public:
    // Для дискретной системы асимптотическая устойчивость требует |z_i| < 1.
    // |z| == 1 — граница (не асимптотически устойчива).
    static StabilityResult analyzeFirstOrder(double a, double tolerance = 1e-12);
};
