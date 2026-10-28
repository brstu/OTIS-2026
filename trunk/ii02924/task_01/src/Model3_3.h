#pragma once
#include "Model.h"

class Model3_3 : public Model {
private:
    double a, b, dt;
    double y_curr = 0.0;

public:
    Model3_3(double a_val, double b_val, double dt_val, double y0 = 0.0)
        : Model("Model 3.3"), a(a_val), b(b_val), dt(dt_val), y_curr(y0) {}

    double step(double u) override {
        double dy = -a * y_curr + b * u;
        y_curr += dt * dy;
        return y_curr;
    }

    void reset() override {
        y_curr = 0.0;
    }
};