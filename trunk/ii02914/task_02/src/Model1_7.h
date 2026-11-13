#pragma once

#include "Model.h"
#include "StabilityAnalyzer.h"

// Model 1.7: System with Multi-Step Control History
// y_{t+1} = a * y_t + b1 * u_t + b2 * u_{t-1} + b3 * u_{t-2}
class Model1_7 : public Model {
private:
    double a;
    double b1;
    double b2;
    double b3;
    double y = 0.0;
    double u_prev1 = 0.0; // control one step back
    double u_prev2 = 0.0; // control two steps back

public:
    Model1_7(double a_, double b1_, double b2_, double b3_)
        : a(a_), b1(b1_), b2(b2_), b3(b3_) {}

    double next(double u) override {
        const double y_next = a * y + b1 * u + b2 * u_prev1 + b3 * u_prev2;
        u_prev2 = u_prev1;
        u_prev1 = u;
        y = y_next;
        return y;
    }

    void reset() override {
        y = 0.0;
        u_prev1 = 0.0;
        u_prev2 = 0.0;
    }

    std::string name() const override {
        return "Model 1.7 (System with Multi-Step Control History)";
    }

    // Stability depends only on the homogeneous part: the characteristic root equals a.
    // In state-space form the eigenvalues are a and two zeros (pure delay).
    bool isStable() const override {
        return stability::isStable(stability::classifyFirstOrder(a));
    }

    std::string stabilityWarning() const override {
        return stability::firstOrderWarning("Model 1.7", a);
    }
};