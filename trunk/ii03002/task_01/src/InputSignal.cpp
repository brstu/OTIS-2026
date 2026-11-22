#include "InputSignal.h"
#include <cmath>
#include <array>

double InputSignal::getSignal(SignalType sig_type, int time_index) {
    if (sig_type == SignalType::STEP) {
        return 1.0;
    }
    if (sig_type == SignalType::IMPULSE) {
        return time_index == 0 ? 1.0 : 0.0;
    }
    if (sig_type == SignalType::HARMONIC) {
        constexpr double freq_multiplier = 0.4;
        return std::sin(freq_multiplier * static_cast<double>(time_index));
    }
    return 0.0;
}

std::string InputSignal::getSignalName(SignalType sig_type) {
    switch (sig_type) {
        case SignalType::STEP: return "Step";
        case SignalType::IMPULSE: return "Impulse";
        case SignalType::HARMONIC: return "Harmonic";
        default: return "Unknown";
    }
}