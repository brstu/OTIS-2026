#pragma once
#include "Model.hpp"

// Линейная модель 1.4
class LinearModel14 : public Model {
    double a;
    double b;
    double y = 0.0;      // текущее y
    double uPrev = 0.0;  // предыдущее u
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

// Нелинейная модель 2.6
class NonlinearModel26 : public Model {
    double a1;
    double a2;
    double b;
    double y = 0.0;      // текущее y
    double yPrev = 0.0;  // предыдущее y
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

// Дифференциальная модель 3.8
class DiffModel38 : public Model {
    double a1;
    double a2;
    double b;
    double dt;
    double y = 0.0;      // текущее y
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
