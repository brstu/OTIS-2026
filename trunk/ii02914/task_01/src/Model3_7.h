#pragma once
#include "Model.h"

class Model3_7 : public Model {
private:
    double param_a;
    double param_b;
    double time_dt;

public:
    explicit Model3_7(double p_a, double p_b, double p_dt, double init_val = 0.0);
    
    auto step(double input_signal) -> double override;
    auto reset(double init_val = 0.0) -> void override;
};