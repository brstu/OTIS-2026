#include "Model2_6.h"

Model2_6::Model2_6(double a1, double a2, double b)
    : a1(a1), a2(a2), b(b)
{
}

double Model2_6::calculateNext(
    double y,
    double yPrev,
    double /*yPrev2*/,
    double u,
    double /*dt*/
) const
{
    // Model 2.6:
    // y(t+1) = a1*y(t)^3 - a2*y(t-1) + b*u(t)^2
    return a1 * y * y * y - a2 * yPrev + b * u * u;
}

const char* Model2_6::getName() const
{
    return "Model 2.6 - Asymmetric Polynomial Backlash Simulation";
}
