#pragma once
#include "Model.h"

class Model1_8 : public Model {
private:
    double a1{0.0};
    double a2{0.0};
    double b1{0.0};
    double b2{0.0};
    double y_prev{0.0};
    double u_prev{0.0};

public:
    explicit Model1_8(double a1_val, double a2_val, double b1_val, double b2_val, double initial_y = 0.0);
    double step(double u) override;
    void reset(double initial_y = 0.0) override;
};