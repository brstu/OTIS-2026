#pragma once
#include "Model.h"

class Model1_8 : public Model {
private:
    double a1;
    double a2;
    double b1;
    double b2;
    double y0_init;
    double y_curr;
    double y_prev;
    double u_prev = 0.0;

public:
    Model1_8(double a1_val, double a2_val, double b1_val, double b2_val, double y0 = 0.0)
        : Model("Model 1.8"), a1(a1_val), a2(a2_val), b1(b1_val), b2(b2_val),
          y0_init(y0), y_curr(y0), y_prev(y0) {}

    double step(double u) override {
        double y_next = a1 * y_curr + a2 * y_prev + b1 * u + b2 * u_prev;
        y_prev = y_curr;
        y_curr = y_next;
        u_prev = u;
        return y_next;
    }

    void reset() override {
        y_curr = y0_init;
        y_prev = y0_init;
        u_prev = 0.0;
    }
};