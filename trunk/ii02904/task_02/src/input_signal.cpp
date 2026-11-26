#include "input_signal.h"

#include <cmath>
#include <stdexcept>

double inputAt(InputSignal signal, int tau, double stepValue)
{
    switch (signal) {
    case InputSignal::Step:
        return stepValue;
    case InputSignal::Impulse:
        return tau == 0 ? 1.0 : 0.0;
    case InputSignal::Harmonic:
        return std::sin(static_cast<double>(tau));
    }

    throw std::invalid_argument("Unknown input signal.");
}
