#pragma once
#include <string_view>

enum class DiscreteSignalPattern {
    HeavisideStep,
    DiracImpulse,
    SinusoidalWave
};

namespace SignalGenerator {
    double sample(DiscreteSignalPattern pattern, int k);
    std::string_view nameOf(DiscreteSignalPattern pattern);
}