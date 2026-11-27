#pragma once
#include "StabilityAnalyzer.h"

class Model_1_8 {
private:
    double a1, a2, b1, b2;
    double y_prev1 = 0.0;
    double y_prev2 = 0.0;
    double u_prev = 0.0;

public:
    Model_1_8(double a1, double a2, double b1, double b2) 
        : a1(a1), a2(a2), b1(b1), b2(b2) {
        StabilityAnalyzer::checkAndWarn(a1, a2);
    }

    void reset(double y0 = 0.0, double y_1 = 0.0) {
        y_prev1 = y0;
        y_prev2 = y_1;
        u_prev = 0.0;
    }

    double step(double u_tau) {
        double y_next = a1 * y_prev1 + a2 * y_prev2 + b1 * u_tau + b2 * u_prev;
        y_prev2 = y_prev1;
        y_prev1 = y_next;
        u_prev = u_tau;
        return y_next;
    }
};