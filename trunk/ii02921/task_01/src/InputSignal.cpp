#include "InputSignal.h"
#include <cmath>

double InputSignal::getSignal(SignalType type, int tau) {
    constexpr double A = 1.0;
    switch (type) {
        case SignalType::STEP:
            return A;
        case SignalType::IMPULSE:
            return (tau == 0) ? A : 0.0;
        case SignalType::HARMONIC:
            return A * std::sin(static_cast<double>(tau));
        default:
            return 0.0;
    }
}

std::string InputSignal::getSignalName(SignalType type) {
    switch (type) {
        case SignalType::STEP:
            return "Step";
        case SignalType::IMPULSE:
            return "Impulse";
        case SignalType::HARMONIC:
            return "Harmonic";
        default:
            return "Unknown";
    }
}