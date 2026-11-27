#ifndef STABILITY_ANALYZER_H
#define STABILITY_ANALYZER_H

#include <iostream>

class StabilityAnalyzer {
public:
    // Проверка корней на Z-плоскости: |z| = |a| < 1
    static bool isStableFirstOrder(double a, bool printWarning = true) {
        if (std::abs(a) >= 1.0) {
            if (printWarning) {
                std::cerr << "[WARNING] Unstable discrete system! Parameter |a| = " 
                          << std::abs(a) << " >= 1.0 (root lies on or outside unit circle on Z-plane)\n";
            }
            return false;
        }
        return true;
    }
};

#endif