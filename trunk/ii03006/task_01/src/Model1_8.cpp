#include "Model1_8.h"

Model1_8::Model1_8(double a1_val, double a2_val, double b1_val, double b2_val, double initial_y)
    : Model(initial_y), a1(a1_val), a2(a2_val), b1(b1_val), b2(b2_val), y_prev(initial_y) {}

double Model1_8::step(double u) {
    const double current_y = getY();
    const double next_y = a1 * current_y + a2 * y_prev + b1 * u + b2 * u_prev;
    y_prev = current_y;
    u_prev = u;
    setY(next_y);
    return next_y;
}

void Model1_8::reset(double initial_y) {
    Model::reset(initial_y);
    y_prev = initial_y;
    u_prev = 0.0;
}