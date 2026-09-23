#include "Model39.h"

Model39::Model39(double y0, double b_, double dt_)
    : y(y0), b(b_), dt(dt_) {
}

double Model39::step(double u, int tau) {
    (void)tau;

    // dy/dt = b * tanh(u)
    // Euler: y(t+dt) = y(t) + dt * b * tanh(u)
    y = y + dt * b * std::tanh(u);

    return y;
}

double Model39::getY() const {
    return y;
}
