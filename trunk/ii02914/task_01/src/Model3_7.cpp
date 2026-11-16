#include "Model3_7.h"
#include <cmath>

Model3_7::Model3_7(double p_a, double p_b, double p_dt, double init_val)
    : Model(init_val), param_a(p_a), param_b(p_b), time_dt(p_dt) {}

auto Model3_7::step(double input_signal) -> double {
    const double current_y = retrieveState();
    
    // Дискретизация непрерывного процесса методом Эйлера для dy/dt = -exp(a)*y + b*u
    const double exp_term = std::exp(param_a);
    const double derivative = (-exp_term * current_y) + (param_b * input_signal);
    
    const double next_y = current_y + (time_dt * derivative);

    assignState(next_y);
    return next_y;
}

auto Model3_7::reset(double init_val) -> void {
    Model::reset(init_val);
}