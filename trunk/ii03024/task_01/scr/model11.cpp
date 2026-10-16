#include "model11.h"

Model11::Model11(double a1, double a2, double b)
    : a1(a1), a2(a2), b(b), y_prev(0.0), y_prev2(0.0) {
}

std::string Model11::getName() const {
    return "Model 11 - Linear Model";
}

void Model11::reset() {
    y = 0.0;
    y_prev = 0.0;
    y_prev2 = 0.0;
}

double Model11::nextStep(double u, double dt) {
    double y_next = a1 * y_prev + a2 * y_prev2 + b * u;
    y_prev2 = y_prev;
    y_prev = y_next;
    y = y_next;
    return y;
}