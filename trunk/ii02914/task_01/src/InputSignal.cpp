#include "InputSignal.h"
#include <cmath>

auto InputSignal::getSignal(SignalType mode, int step_k) -> double {
    switch (mode) {
        case SignalType::STEP:
            return 1.0;
        case SignalType::IMPULSE:
            return step_k == 0 ? 1.0 : 0.0;
        case SignalType::HARMONIC:
            return std::sin(0.4 * step_k);
    }
    return 0.0;
}

auto InputSignal::getSignalName(SignalType mode) -> std::string {
    switch (mode) {
        case SignalType::STEP:
            return "Step";
        case SignalType::IMPULSE:
            return "Impulse";
        case SignalType::HARMONIC:
            return "Harmonic";
    }
    return "Unknown";
}
