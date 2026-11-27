#include "DiffEquations.h"
#include <cmath>
#include <stdexcept>

Model3_8::Model3_8(double a1, double a2, double b, double dt)
    : a1_(a1), a2_(a2), b_(b), dt_(dt)
{
    if (!std::isfinite(dt) || dt <= 0.0)
        throw std::invalid_argument("Euler step must be positive and finite");
}

std::string Model3_8::name() const
{
    return "Model 3.8 (Combined Linear-Quadratic Decay)";
}

std::string Model3_8::formula() const
{
    return "dy/dt = -a1*y - a2*y^2 + b*u  (Euler, step dt)";
}

double Model3_8::calculateNext(double u) const
{
    const double y = currentY();
    const double dydt = -a1_ * y - a2_ * y * y + b_ * u;
    return y + dt_ * dydt;
}
