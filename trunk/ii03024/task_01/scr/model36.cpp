#include "model36.h"

Model36::Model36(double a, double b) : a(a), b(b) {}

std::string Model36::getName() const {
    return "Model 36 - Differential Model";
}

double Model36::nextStep(double u, double dt) {
    y = y + dt * (-std::exp(a) * y + b * u);
    return y;
}