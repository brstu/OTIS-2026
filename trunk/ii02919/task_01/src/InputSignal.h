#pragma once

#include <string>

// ============================================================================
//  InputSignal — абстрактный базовый класс входного воздействия
//  Определяет метод value(tau) — значение сигнала на шаге tau.
// ============================================================================
class InputSignal
{
public:
    virtual ~InputSignal() = default;

    // Значение сигнала на шаге tau (tau начинается с 1).
    virtual double value(int tau) const = 0;

    // Название сигнала (для вывода в консоль).
    virtual std::string label() const = 0;
};

// ----------------------------------------------------------------------------
//  UnitStep — ступенчатое воздействие: u(tau) = amplitude для всех tau
// ----------------------------------------------------------------------------
class UnitStep : public InputSignal
{
private:
    double amplitude;

public:
    explicit UnitStep(double amp = 1.0);

    double value(int tau) const override;
    std::string label() const override;
};

// ----------------------------------------------------------------------------
//  SingleImpulse — импульсное воздействие:
//    u(1) = 1, u(tau > 1) = 0
// ----------------------------------------------------------------------------
class SingleImpulse : public InputSignal
{
public:
    double value(int tau) const override;
    std::string label() const override;
};

// ----------------------------------------------------------------------------
//  Sinusoid — гармоническое воздействие: u(tau) = sin(tau)
// ----------------------------------------------------------------------------
class Sinusoid : public InputSignal
{
public:
    double value(int tau) const override;
    std::string label() const override;
};
