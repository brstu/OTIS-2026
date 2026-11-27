#include "Simulation.h"
#include "StabilityAnalyzer.h"
#include <cmath>

// Model1_1
Model1_1::Model1_1(double a_val, double b_val, double initial_y)
    : Model(initial_y), a(a_val), b(b_val) {
    StabilityAnalyzer::isStableFirstOrder(a, true);
}

double Model1_1::step(double u) {
    double next_y = a * getY() + b * u;
    setY(next_y);
    return next_y;
}

// Model2_5
Model2_5::Model2_5(double a_val, double b_val, double initial_y)
    : Model(initial_y), a(a_val), b(b_val) {}

double Model2_5::sign(double val) {
    if (val > 0.0) return 1.0;
    if (val < 0.0) return -1.0;
    return 0.0;
}

double Model2_5::step(double u) {
    double next_y = a * getY() + b * sign(u) * (1.0 - std::exp(-std::abs(u)));
    setY(next_y);
    return next_y;
}

// Model3_9
Model3_9::Model3_9(double b_val, double dt_val, double initial_y)
    : Model(initial_y), b(b_val), dt(dt_val) {}

double Model3_9::step(double u) {
    double dy = b * std::tanh(u);
    double next_y = getY() + dy * dt;
    setY(next_y);
    return next_y;
}