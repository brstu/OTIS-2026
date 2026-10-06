#pragma once

#include <cmath>
#include <string>

// ============================================================================
//  InputSignal — абстрактный базовый класс входного воздействия
// ============================================================================
class InputSignal
{
public:
    virtual ~InputSignal() = default;

    virtual double value(int tau) const = 0;
    virtual std::string label() const = 0;
};

// ============================================================================
//  UnitStep — ступенчатое воздействие: u(tau) = amplitude
// ============================================================================
class UnitStep : public InputSignal
{
private:
    double amplitude = 1.0;

public:
    explicit UnitStep(double amp) : amplitude(amp) {}

    double value(int /*tau*/) const override
    {
        return amplitude;
    }

    std::string label() const override
    {
        return "Step (u = const)";
    }
};

// ============================================================================
//  SingleImpulse — импульс на tau = 1: u(1) = 1, u(tau > 1) = 0
// ============================================================================
class SingleImpulse : public InputSignal
{
public:
    double value(int tau) const override
    {
        return (tau == 1) ? 1.0 : 0.0;
    }

    std::string label() const override
    {
        return "Impulse (u_1 = 1, u_tau>1 = 0)";
    }
};

// ============================================================================
//  Sinusoid — гармоническое воздействие: u(tau) = sin(tau)
// ============================================================================
class Sinusoid : public InputSignal
{
public:
    double value(int tau) const override
    {
        return std::sin(static_cast<double>(tau));
    }

    std::string label() const override
    {
        return "Harmonic (u_tau = sin(tau))";
    }
};
