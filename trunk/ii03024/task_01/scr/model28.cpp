#include "model28.h"

Model28::Model28(double a, double b) : a(a), b(b) {}

std::string Model28::getName() const {
    return "Model 28 - Nonlinear Model";
}

double Model28::nextStep(double u, double dt) {
    y = a * y + b * deadzone(u);
    return y;
}