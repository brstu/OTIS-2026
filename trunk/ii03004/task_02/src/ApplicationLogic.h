#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include "StabilityAnalyzer.h"

// Базовый класс для входных сигналов
class Input {
public:
    virtual ~Input() = default;
    virtual double value(int tau) const = 0;
    virtual std::string name() const = 0;
};

class StepInput : public Input {
    double c;
public:
    explicit StepInput(double c = 1.0) : c(c) {}
    double value(int) const override { return c; }
    std::string name() const override { return "step"; }
};

class ImpulseInput : public Input {
public:
    double value(int tau) const override { return tau == 0 ? 1.0 : 0.0; }
    std::string name() const override { return "impulse"; }
};

class HarmonicInput : public Input {
public:
    double value(int tau) const override { return std::sin(static_cast<double>(tau)); }
    std::string name() const override { return "harmonic"; }
};

// Базовый класс модели
class Model {
    double y;
    double y0;
protected:
    void setY(double value) { y = value; }
public:
    explicit Model(double start = 0.0) : y(start), y0(start) {}
    virtual ~Model() = default;
    virtual double next(double u) = 0;
    virtual std::string name() const = 0;
    virtual void reset() { y = y0; }
    double current() const { return y; }
};

// Линейная модель (с проверкой через StabilityAnalyzer)
class LinearModel : public Model {
    double a1;
    double b1;
    double b2;
    double u1 = 0.0;
    double u2 = 0.0;
public:
    LinearModel(double a1, double b1, double b2, double y0 = 0.0)
        : Model(y0), a1(a1), b1(b1), b2(b2) {}

    double next(double u) override {
        setY(a1 * current() + b1 * u + b2 * u2);
        u2 = u1;
        u1 = u;
        return current();
    }

    void reset() override {
        Model::reset();
        u1 = 0.0;
        u2 = 0.0;
    }

    std::string name() const override { return "Model 1.10 (linear)"; }

    bool isStable() const {
    return StabilityAnalyzer::isLinearSystemStable(a1);
}
};

// Нелинейная модель
class NonlinearModel : public Model {
    double a;
    double b;
public:
    NonlinearModel(double a, double b, double y0 = 0.0) : Model(y0), a(a), b(b) {}
    double next(double u) override {
        double y = current();
        setY(a * y * std::cos(y) + b * std::log(1.0 + std::fabs(u)));
        return current();
    }
    std::string name() const override { return "Model 2.4 (nonlinear)"; }
};

// Дифференциальная модель (метод Эйлера)
class DiffModel : public Model {
    double a1;
    double a2;
    double b;
    double dt;
public:
    DiffModel(double a1, double a2, double b, double dt, double y0 = 0.0)
        : Model(y0), a1(a1), a2(a2), b(b), dt(dt) {}
    double next(double u) override {
        double y = current();
        setY(y + dt * (-a1 * y - a2 * y * y + b * u));
        return current();
    }
    std::string name() const override { return "Model 3.8 (Euler)"; }
};