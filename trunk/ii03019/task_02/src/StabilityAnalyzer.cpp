#include "StabilityAnalyzer.h"
#include <cmath>

bool StabilityAnalyzer::isStable(double a) {
    return std::abs(a) < 1.0;
}