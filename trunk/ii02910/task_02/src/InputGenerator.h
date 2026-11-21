#pragma once

#include <string>

// Базовый интерфейс генератора входного сигнала
class InputSignal
{
public:
    virtual ~InputSignal() = default;
    virtual double getValue(int tau) const = 0;
    virtual std::string getLabel() const = 0;
};

// Постоянный сигнал: u(tau) = level
class ConstantInput : public InputSignal
{
private:
    double level;

public:
    explicit ConstantInput(double level_ = 1.0) : level(level_) {}

    double getValue(int tau) const override;
    std::string getLabel() const override;
};

// Импульсный сигнал: u(1) = 1, u(tau>1) = 0
class PulseInput : public InputSignal
{
public:
    double getValue(int tau) const override;
    std::string getLabel() const override;
};

// Гармонический сигнал: u(tau) = sin(tau)
class SineInput : public InputSignal
{
public:
    double getValue(int tau) const override;
    std::string getLabel() const override;
};