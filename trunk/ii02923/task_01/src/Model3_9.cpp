#include "Model3_9.h"

#include <cmath>

Model3_9::Model3_9(double b) : b_(b) {}

void Model3_9::reset() {
    y_ = 0.0;
}

double Model3_9::step(double u, double dt) {
    y_ = y_ + dt * b_ * std::tanh(u);
    return y_;
}

std::string Model3_9::name() const {
    return "Model 3.9";
}

std::string Model3_9::equation() const {
    return "dy/dt = b*tanh(u);  y[t+1] = y[t] + dt*b*tanh(u[t])";
}

bool Model3_9::isStable() const {
    // Правая часть не зависит от y — это не рекуррентная модель,
    // понятие устойчивости дискретной системы к ней неприменимо.
    return true;
}
