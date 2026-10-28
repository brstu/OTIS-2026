#pragma once
#include "Model.h"

class Model2_3 : public Model {
private:
    double a;
    double b;
    double delta;
    double y_curr = 0.0;

    double deadzone(double u) const {
        if (u > delta) {
            return u - delta;
        } else if (u < -delta) {
            return u + delta;
        }
        return 0.0;
    }

public:
    Model2_3(double a_val, double b_val, double delta_val, double y0 = 0.0)
        : Model("Model 2.3"), a(a_val), b(b_val), delta(delta_val), y_curr(y0) {}

    double step(double u) override {
        double y_next = a * y_curr + b * deadzone(u);
        y_curr = y_next;
        return y_next;
    }

    void reset() override {
        y_curr = 0.0;
    }
};