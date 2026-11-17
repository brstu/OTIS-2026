#pragma once
#include "Model.h"

class Model2_3 : public Model {
private:
    double alpha;
    double beta;
    double delta;

public:
    explicit Model2_3(double p_a, double p_b, double p_delta, double init_val = 0.0);
    
    auto step(double input_signal) -> double override;
};