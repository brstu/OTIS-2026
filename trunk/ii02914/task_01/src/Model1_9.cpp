#include "Model1_9.h"

Model1_9::Model1_9(double p_a1, double p_a2, double p_a3, double p_b1, double init_val)
    : Model(init_val),
      a1_val(p_a1), a2_val(p_a2), a3_val(p_a3), b1_val(p_b1),
      y_prev1(init_val), y_prev2(init_val) {}

auto Model1_9::step(double input_signal) -> double {
    const double current_y = retrieveState();
    
    // Разделяем формулу на независимые слагаемые для изменения AST-структуры
    const double part1 = a1_val * current_y;
    const double part2 = a2_val * y_prev1;
    const double part3 = a3_val * y_prev2;
    const double input_part = b1_val * input_signal;
    
    const double next_y = part1 + part2 + part3 + input_part;
    
    // Сдвиг истории состояний
    y_prev2 = y_prev1;
    y_prev1 = current_y;
    
    assignState(next_y);
    return next_y;
}

auto Model1_9::reset(double init_val) -> void {
    Model::reset(init_val);
    y_prev1 = init_val;
    y_prev2 = init_val;
}