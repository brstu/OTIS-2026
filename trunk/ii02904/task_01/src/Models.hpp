#pragma once
#include "Model.hpp"

// Model 1.4: y(t+1) = a*y(t) + b*(u(t) - u(t-1))
class LinearModel14 : public Model {
    double a, b;
    double y = 0.0;      // y(t)
    double uPrev = 0.0;  // u(t-1)
public:
    LinearModel14(double a, double b) : a(a), b(b) {}

    double nextStep(double u) override {
        y = a * y + b * (u - uPrev);
        uPrev = u;
        return y;
    }
    std::string name() const override { return "Model 1.4"; }
    double current() const override { return y; }
};

// Model 2.6: y(t+1) = a1*y(t)^3 - a2*y(t-1) + b*u(t)^2
class NonlinearModel26 : public Model {
    double a1, a2, b;
    double y = 0.0;      // y(t)
    double yPrev = 0.0;  // y(t-1)
public:
    NonlinearModel26(double a1, double a2, double b) : a1(a1), a2(a2), b(b) {}

    double nextStep(double u) override {
        double yNew = a1 * y * y * y - a2 * yPrev + b * u * u;
        yPrev = y;
        y = yNew;
        return y;
    }
    std::string name() const override { return "Model 2.6"; }
    double current() const override { return y; }
};

// Model 3.8: dy/dt = -a1*y - a2*y^2 + b*u, метод Эйлера
class DiffModel38 : public Model {
    double a1, a2, b, dt;
    double y = 0.0;      // y(t)
public:
    DiffModel38(double a1, double a2, double b, double dt)
        : a1(a1), a2(a2), b(b), dt(dt) {}

    double nextStep(double u) override {
        y = y + dt * (-a1 * y - a2 * y * y + b * u);
        return y;
    }
    std::string name() const override { return "Model 3.8"; }
    double current() const override { return y; }
};
