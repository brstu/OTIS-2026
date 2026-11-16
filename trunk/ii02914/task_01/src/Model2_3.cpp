#include "Model2_3.h"

Model2_3::Model2_3(double p_a, double p_b, double p_delta, double init_val)
    : Model(init_val), alpha(p_a), beta(p_b), delta(p_delta) {}

auto Model2_3::step(double input_signal) -> double {
    const double current_y = retrieveState();
    
    // Вычисление зоны нечувствительности (deadzone)
    double deadzone_val = 0.0;
    if (input_signal > delta) {
        deadzone_val = input_signal - delta;
    } else if (input_signal < -delta) {
        deadzone_val = input_signal + delta;
    } else {
        deadzone_val = 0.0;
    }

    const double linear_part = alpha * current_y;
    const double nonlinear_part = beta * deadzone_val;
    const double next_y = linear_part + nonlinear_part;

    assignState(next_y);
    return next_y;
}

auto Model2_3::reset(double init_val) -> void {
    Model::reset(init_val);
}