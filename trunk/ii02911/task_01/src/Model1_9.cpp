#include "Model1_9.h"

Model1_9::Model1_9(double a1, double a2, double a3, double b1)
    : a1(a1), a2(a2), a3(a3), b1(b1)
{
}

double Model1_9::calculateNext(
    double y,
    double yPrev,
    double yPrev2,
    double u,
    double /*dt*/
) const
{
    // Model 1.9:
    // y(t+1) = a1*y(t) + a2*y(t-1) + a3*y(t-2) + b1*u(t)
    return a1 * y + a2 * yPrev + a3 * yPrev2 + b1 * u;
}

const char* Model1_9::getName() const
{
    return "Model 1.9 - Three-Step Moving Average State Model";
}
