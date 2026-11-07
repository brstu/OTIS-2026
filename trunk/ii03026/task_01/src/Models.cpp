#include "Models.h"

// LinearModel1_1
LinearModel1_1::LinearModel1_1(double a, double b) : a(a), b(b) {}

double LinearModel1_1::calculateNextStep(double u, double y_prev) {
    return a * y_prev + b * u;
}

// NonLinearModel2_3
NonLinearModel2_3::NonLinearModel2_3(double a, double b, double delta) 
    : a(a), b(b), delta(delta) {}

double NonLinearModel2_3::deadzone(double u) {
    return (std::abs(u) > delta) ? (u > 0 ? u - delta : u + delta) : 0.0;
}

double NonLinearModel2_3::calculateNextStep(double u, double y_prev) {
    double u_dead = deadzone(u);
    return a * y_prev + b * u_dead;
}

// DiffModel3_5
DiffModel3_5::DiffModel3_5(double b, double dt) : b(b), dt(dt) {}

double DiffModel3_5::calculateNextStep(double u, double y_prev) {
    return y_prev + dt * (b * std::sin(u));
}