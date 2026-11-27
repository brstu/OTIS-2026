#pragma once
#include "Model.h"

class Model3_3 : public Model {
private:
    double a{0.0};
    double b{0.0};
    double dt{0.1};

public:
    explicit Model3_3(double a_val, double b_val, double dt_val = 0.1, double initial_y = 0.0);
    double step(double u) override;
};