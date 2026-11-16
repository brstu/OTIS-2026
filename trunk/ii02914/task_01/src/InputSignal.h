#pragma once
#include <string>

enum class SignalType : short {
    STEP = 10,
    IMPULSE = 20,
    HARMONIC = 30
};

struct InputSignal {
    static auto getSignal(SignalType mode, int step_k) -> double;
    static auto getSignalName(SignalType mode) -> std::string;
};