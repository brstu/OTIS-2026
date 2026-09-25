#include "InputSignal.h"
#include <cmath>

// ============================================================================
//  UnitStep — ступенчатое воздействие
// ============================================================================
UnitStep::UnitStep(double amp)
    : amplitude(amp)
{
}

double UnitStep::value(int /*tau*/) const
{
    // Ступенька: значение постоянно для всех tau
    return amplitude;
}

std::string UnitStep::label() const
{
    return "Step (u = const)";
}

// ============================================================================
//  SingleImpulse — одиночный импульс на tau = 1
// ============================================================================
double SingleImpulse::value(int tau) const
{
    // Импульс: u(1) = 1, все остальные = 0
    return (tau == 1) ? 1.0 : 0.0;
}

std::string SingleImpulse::label() const
{
    return "Impulse (u_1 = 1, u_tau>1 = 0)";
}

// ============================================================================
//  Sinusoid — гармоническое воздействие
// ============================================================================
double Sinusoid::value(int tau) const
{
    // u(tau) = sin(tau)
    return std::sin(static_cast<double>(tau));
}

std::string Sinusoid::label() const
{
    return "Harmonic (u_tau = sin(tau))";
}
