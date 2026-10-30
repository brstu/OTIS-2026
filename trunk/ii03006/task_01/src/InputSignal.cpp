#include "InputSignal.h"
#include <cmath>

namespace SignalGenerator {

double sample(DiscreteSignalPattern pattern, int k) {
    switch (pattern) {
        case DiscreteSignalPattern::HeavisideStep:
            return 1.0;
        case DiscreteSignalPattern::DiracImpulse:
            return (k == 0) ? 1.0 : 0.0;
        case DiscreteSignalPattern::SinusoidalWave:
            return std::sin(0.4 * static_cast<double>(k));
        default:
            return 0.0;
    }
}

std::string_view nameOf(DiscreteSignalPattern pattern) {
    if (pattern == DiscreteSignalPattern::DiracImpulse) {
        return "Impulse";
    }
    if (pattern == DiscreteSignalPattern::SinusoidalWave) {
        return "Harmonic";
    }
    return "Step";
}

} // namespace SignalGenerator