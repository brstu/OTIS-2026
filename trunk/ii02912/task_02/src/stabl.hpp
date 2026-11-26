
#pragma once
#include <cmath>

class StabilityAnalyzer {
public:
    struct Result {
        double root;
        bool stable;
    };

    static Result analyze(double a) {
        double root = a;

        bool stable = std::isfinite(root) && std::abs(root) < 1.0;

        return { root, stable };
    }
};
