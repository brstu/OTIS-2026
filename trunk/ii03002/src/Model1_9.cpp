#include "Model1_9.h"

Model1_9::Model1_9(double a1_val, double a2_val, double a3_val, double b1_val, double initial_y)
    : Model(initial_y), a1(a1_val), a2(a2_val), a3(a3_val), b1(b1_val),
      y_prev1(initial_y), y_prev2(initial_y) {}

double Model1_9::step(double u) {
    const double current_y = getY();
    const double next_y = a1 * current_y + a2 * y_prev1 + a3 * y_prev2 + b1 * u;
    y_prev2 = y_prev1;
    y_prev1 = current_y;
    setY(next_y);
    return next_y;
}

void Model1_9::reset(double initial_y) {
    Model::reset(initial_y);
    y_prev1 = initial_y;
    y_prev2 = initial_y;
}