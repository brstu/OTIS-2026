#include "StabilityAnalyzer.h"
#include <cmath>
#include <stdexcept>

PoleLocation locatePole(double a)
{
    if (!std::isfinite(a))
        throw std::invalid_argument("Coefficient a must be finite");
    const double radius = std::abs(a);
    if (radius < 1.0)
        return PoleLocation::Inside;
    if (radius == 1.0)
        return PoleLocation::Boundary;
    return PoleLocation::Outside;
}
