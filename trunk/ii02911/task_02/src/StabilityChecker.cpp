#include "StabilityChecker.h"
#include <cmath>

bool StabilityChecker::isModel1_9Stable(double a1, double a2, double a3)
{
    if (!std::isfinite(a1) || !std::isfinite(a2) || !std::isfinite(a3))
    {
        return false;
    }

    // Приводим к стандартному виду P(z) = z^3 + p2*z^2 + p1*z + p0
    const double p2 = -a1;
    const double p1 = -a2;
    const double p0 = -a3;

    // Необходимые и достаточные условия Джури для полинома 3-й степени (все |z_i| < 1):
    // 1) P(1) > 0
    if (const double p_at_1 = 1.0 + p2 + p1 + p0; p_at_1 <= 0.0)
    {
        return false;
    }

    // 2) (-1)^3 * P(-1) > 0  =>  1 - p2 + p1 - p0 > 0
    if (const double p_at_minus_1 = 1.0 - p2 + p1 - p0; p_at_minus_1 <= 0.0)
    {
        return false;
    }

    // 3) |p0| < 1
    if (std::abs(p0) >= 1.0)
    {
        return false;
    }

    // 4) |p0^2 - 1| > |p0*p2 - p1|
    const double lhs = 1.0 - p0 * p0;
    const double rhs = std::abs(p0 * p2 - p1);
    return lhs > rhs;
}

bool StabilityChecker::checkAndWarn(double a1, double a2, double a3, std::ostream& out)
{
    const bool stable = isModel1_9Stable(a1, a2, a3);
    if (!stable)
    {
        out << "[ПРЕДУПРЕЖДЕНИЕ] Линейная модель 1.9 неустойчива при заданных коэффициентах "
            << "(a1=" << a1 << ", a2=" << a2 << ", a3=" << a3 << ").\n";
    }
    return stable;
}
