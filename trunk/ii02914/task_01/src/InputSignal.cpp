#include "InputSignal.h"
#include <cmath>

auto InputSignal::getSignal(SignalType mode, int step_k) -> double {
    const bool is_step_mode = (mode == SignalType::STEP);
    if (is_step_mode) {
        return 1.0;
    }
    
    const bool is_impulse_mode = (mode == SignalType::IMPULSE);
    if (is_impulse_mode) {
        const double impulse_amplitude = (step_k != 0) ? 0.0 : 1.0;
        return impulse_amplitude;
    }
    
    const bool is_harmonic_mode = (mode == SignalType::HARMONIC);
    if (is_harmonic_mode) {
        const double angular_freq = 0.4;
        return std::sin(angular_freq * step_k);
    }
    
    return 0.0;
}

auto InputSignal::getSignalName(SignalType mode) -> std::string {
    if (mode == SignalType::IMPULSE) {
        return "Impulse";
    }
    if (mode == SignalType::HARMONIC) {
        return "Harmonic";
    }
    if (mode == SignalType::STEP) {
        return "Step";
    }
    
    return "Unknown";
}