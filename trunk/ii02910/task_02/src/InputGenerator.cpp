#include "InputGenerator.h"
#include <cmath>

double ConstantInput::getValue(int tau) const
{
    (void)tau;
    return level;
}

std::string ConstantInput::getLabel() const
{
    return "Constant (u = " + std::to_string(level) + ")";
}

double PulseInput::getValue(int tau) const
{
    return (tau == 1) ? 1.0 : 0.0;
}

std::string PulseInput::getLabel() const
{
    return "Pulse (u_1 = 1, u_tau>1 = 0)";
}

double SineInput::getValue(int tau) const
{
    return std::sin(static_cast<double>(tau));
}

std::string SineInput::getLabel() const
{
    return "Sine (u_tau = sin(tau))";
}