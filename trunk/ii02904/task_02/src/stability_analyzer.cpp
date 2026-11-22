#include "stability_analyzer.h"

#include <cmath>
#include <stdexcept>

bool StabilityAnalyzer::isModel14Stable(double a)
{
    if (!std::isfinite(a)) {
        throw std::invalid_argument("The model coefficient must be finite.");
    }

    // The homogeneous characteristic equation is z - a = 0.
    return std::abs(a) < 1.0;
}
