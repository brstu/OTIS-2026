#pragma once
#include <cmath>
#include <deque>
#include "Model.h"

// Model 1.5 (система с запаздыванием состояния):
// y(tau+1) = a1*y(tau) + a2*y(tau-k) + b*u(tau)
class Model1_5 : public Model {
public:
    Model1_5(double a1, double a2, double b, int k, double y0)
        : a1_(a1), a2_(a2), b_(b), history_(static_cast<size_t>(k) + 1, y0) {
    }

    double calculateNext(double u) override {
        // history_.back() = y(tau), history_.front() = y(tau-k)
        const double next = a1_ * history_.back() + a2_ * history_.front() + b_ * u;
        history_.push_back(next);
        history_.pop_front();
        return next;
    }

    double current() const override { return history_.back(); }

    std::string getName() const override { return "Model 1.5 (линейная, запаздывание состояния)"; }

    // Достаточное условие устойчивости: |a1| + |a2| < 1
    bool isStable() const override { return std::fabs(a1_) + std::fabs(a2_) < 1.0; }

private:
    double a1_;
    double a2_;
    double b_;
    std::deque<double> history_;
};
