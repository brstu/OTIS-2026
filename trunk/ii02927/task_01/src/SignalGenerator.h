#pragma once
#include <cmath>
#include <string>

// Базовый класс для всех сигналов
class InputSignal
{
public:
    virtual ~InputSignal() = default;
    virtual double value(int tau) const = 0;
    virtual std::string name() const = 0;
};

// Ступенчатый сигнал (константа)
class StepSignal : public InputSignal
{
private:
    double level;
public:
    explicit StepSignal(double amp = 1.0) : level(amp) {}
    double value(int /*tau*/) const override { return level; }
    std::string name() const override { return "Step (u = const)"; }
};

// Импульсный сигнал
class ImpulseSignal : public InputSignal
{
public:
    double value(int tau) const override { return (tau == 1) ? 1.0 : 0.0; }
    std::string name() const override { return "Impulse (u_1 = 1, others 0)"; }
};

// Гармонический (синус)
class HarmonicSignal : public InputSignal
{
public:
    double value(int tau) const override { return std::sin(static_cast<double>(tau)); }
    std::string name() const override { return "Harmonic (u = sin(tau))"; }
};