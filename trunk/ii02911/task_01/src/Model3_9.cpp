#include "Model3_9.h"

#include <cmath>

Model3_9::Model3_9(double b)
    : b(b)
{
}

double Model3_9::calculateNext(
    double y,
    double /*yPrev*/,
    double /*yPrev2*/,
    double u,
    double dt
) const
{
    // Differential equation:
    // dy/dt = b*tanh(u)
    //
    // Euler method:
    // y(t+1) = y(t) + dt*b*tanh(u(t))
    return y + dt * b * std::tanh(u);
}

const char* Model3_9::getName() const
{
    return "Model 3.9 - Bounded Saturation Rate";
}
