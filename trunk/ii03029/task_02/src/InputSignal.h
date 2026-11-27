#pragma once
#include <cmath>

class InputSignal {
public:
    static double step(int t) { return (t >= 0) ? 1.0 : 0.0; }
    static double impulse(int t) { return (t == 0) ? 1.0 : 0.0; }
    static double harmonic(int t, double omega = 0.1) { return std::sin(omega * t); }
};