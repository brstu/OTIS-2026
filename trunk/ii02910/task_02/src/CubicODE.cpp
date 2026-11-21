#include "CubicODE.h"
#include <cmath>
#include <sstream>

CubicODE::CubicODE(double a_, double b_, double dt_)
    : a(a_), b(b_), dt(dt_)
{
}

double CubicODE::step(double u)
{
    double derivative = a * currentY * currentY * currentY + b * u;
    currentY = currentY + dt * derivative;
    return currentY;
}

void CubicODE::reset()
{
    currentY = 1.0;
}

std::string CubicODE::getDescription() const
{
    return "Model 3.6 (Cubic Growth and Control)";
}

bool CubicODE::checkStability() const
{
    const double yRef = 1.0;
    const double deriv = 3.0 * a * yRef * yRef;
    const double z = 1.0 + dt * deriv;
    return std::abs(z) < 1.0;
}

std::string CubicODE::getWarningMessage() const
{
    if (checkStability()) return "";
    std::ostringstream oss;
    const double yRef = 1.0;
    const double deriv = 3.0 * a * yRef * yRef;
    const double z = 1.0 + dt * deriv;
    oss << "[WARNING] Model 3.6: |1 + 3a(y*)²Δt| = " << std::abs(z) << " >= 1.";
    return oss.str();
}