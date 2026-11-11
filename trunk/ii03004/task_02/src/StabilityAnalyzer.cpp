#include "StabilityAnalyzer.h"
#include <cmath>

bool StabilityAnalyzer::isLinearSystemStable(double a1) {
    return std::fabs(a1) < 1.0;
}

double StabilityAnalyzer::calculateRoot(double a1) {
    return a1;
}