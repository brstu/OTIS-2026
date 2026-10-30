#include "Model3_3.h"

Model3_3::Model3_3(double a_val, double b_val, double dt_val, double initial_y)
    : Model(initial_y), a(a_val), b(b_val), dt(dt_val) {}

double Model3_3::step(double u) {
    const double current_y = currentState();
    const double derivative = -a * current_y + b * u;
    const double next_y = current_y + derivative * dt;
    updateState(next_y);
    return next_y;
}