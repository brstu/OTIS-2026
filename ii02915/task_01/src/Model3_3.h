#pragma once
#include <cmath>
#include "Model.h"

// Model 3.3 (дифференциальное уравнение): dy/dt = -a*y + b*u
// Метод Эйлера: y(tau+1) = y(tau) + dt * (-a*y(tau) + b*u(tau))
class Model3_3 : public Model {
public:
    Model3_3(double a, double b, double dt, double y0) : a_(a), b_(b), dt_(dt), y_(y0) {
    }

    double calculateNext(double u) override {
        y_ += dt_ * (-a_ * y_ + b_ * u);
        return y_;
    }

    double current() const override { return y_; }

    std::string getName() const override { return "Model 3.3 (дифф. уравнение, Эйлер)"; }

    // Схема Эйлера: y(tau+1) = (1 - a*dt)*y(tau) + ..., устойчива при |1 - a*dt| < 1
    bool isStable() const override { return std::fabs(1.0 - a_ * dt_) < 1.0; }

private:
    double a_;
    double b_;
    double dt_;
    double y_;
};
