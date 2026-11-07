#pragma once
#include <cmath>
#include "Model.h"

// Model 2.9 (нелинейная, корень с сохранением знака):
// y(tau+1) = a*y(tau) + b*sqrt(|u(tau)|)*sign(u(tau))
class Model2_9 : public Model {
public:
    Model2_9(double a, double b, double y0) : a_(a), b_(b), y_(y0) {
    }

    double calculateNext(double u) override {
        y_ = a_ * y_ + b_ * std::copysign(std::sqrt(std::fabs(u)), u);
        return y_;
    }

    double current() const override { return y_; }

    std::string getName() const override { return "Model 2.9 (нелинейная, корень)"; }

    // Для ограниченного входа выход ограничен, если |a| < 1
    bool isStable() const override { return std::fabs(a_) < 1.0; }

private:
    double a_;
    double b_;
    double y_;
};
