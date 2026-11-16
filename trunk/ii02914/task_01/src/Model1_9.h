#pragma once
#include "Model.h"

class Model1_9 : public Model {
private:
    double a1_val;
    double a2_val;
    double a3_val;
    double b1_val;
    double y_prev1;
    double y_prev2;

public:
    explicit Model1_9(double p_a1, double p_a2, double p_a3, double p_b1, double init_val = 0.0);
    
    auto step(double input_signal) -> double override;
    auto reset(double init_val = 0.0) -> void override;
};