#pragma once

#include "Model.h"
#include "StabilityAnalyzer.h"

// Model 3.1: Pure Linear Decay
// dy/dt = -a * y => y_{t+1} = y_t + dt * (-a * y_t)   (метод Эйлера)
class Model3_1 : public Model {
private:
    static constexpr double DEFAULT_DT = 0.01;
    static constexpr double INITIAL_Y = 1.0;

    double a;
    double dt;
    double y = INITIAL_Y;

public:
    explicit Model3_1(double a_, double dt_ = DEFAULT_DT)
        : a(a_), dt(dt_) {}

    double next(double /*u*/) override {
        const double dy = -a * y;
        y = y + dt * dy;
        return y;
    }

    void reset() override {
        y = INITIAL_Y;
    }

    std::string name() const override {
        return "Model 3.1 (Pure Linear Decay)";
    }

    // Схема Эйлера: z = 1 - a*dt, условие |1 - a*dt| < 1  <=>  0 < a*dt < 2
    bool isStable() const override {
        return stability::isStable(stability::classifyEuler(a, dt));
    }

    std::string stabilityWarning() const override {
        return stability::eulerWarning("Model 3.1", a, dt);
    }
};
