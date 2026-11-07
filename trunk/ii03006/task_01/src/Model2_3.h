#pragma once
#include "Model.h"

class Model2_3 : public Model {
private:
    double a{0.0};
    double b{0.0};
    double delta{0.0};

    static double deadzone(double u, double d);

public:
    explicit Model2_3(double a_val, double b_val, double delta_val, double initial_y = 0.0);
    double step(double u) override;
};