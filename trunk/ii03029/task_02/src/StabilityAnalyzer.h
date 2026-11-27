#pragma once
#include <cmath>
#include <iostream>

class StabilityAnalyzer {
public:
    static bool isStableLinear2ndOrder(double a1, double a2) {
        bool cond1 = (1.0 - a1 - a2 > 0.0);
        bool cond2 = (1.0 + a1 - a2 > 0.0);
        bool cond3 = (std::abs(a2) < 1.0);
        return cond1 && cond2 && cond3;
    }

    static void checkAndWarn(double a1, double a2) {
        if (!isStableLinear2ndOrder(a1, a2)) {
            std::cout << "[WARNING] Coefficients a1 = " << a1 
                      << ", a2 = " << a2 << " lead to an UNSTABLE process!\n";
        }
    }
};