#include "Model2_3.h"

Model2_3::Model2_3(double a_val, double b_val, double delta_val, double initial_y)
    : Model(initial_y), a(a_val), b(b_val), delta(delta_val) {}

double Model2_3::deadzone(double u, double d) {
    if (u > d) {
        return u - d;
    }
    if (u < -d) {
        return u + d;
    }
    return 0.0;
}

double Model2_3::step(double u) {
    const double nonlin = deadzone(u, delta);
    const double next_y = a * currentState() + b * nonlin;
    updateState(next_y);
    return next_y;
}