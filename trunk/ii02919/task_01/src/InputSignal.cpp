#include "InputSignal.h"
#include <cmath>    // для std::sin

// =====================================================================
// Метод value для класса Sinusoid.
// Возвращает sin(tau). Приводим tau к double, чтобы использовать
// правильную перегрузку std::sin.
// =====================================================================
double Sinusoid::value(int tau) const
{
    return std::sin(static_cast<double>(tau));
}
